// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Connection.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommServer.h"
#include "Player.h"
#include "ExternalMind.h"
#include "Persistance.h"

#include "rulesets/Character.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/inheritance.h"
#include "common/system.h"
#include "common/BaseWorld.h"
#include "common/Database.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Info;

static const bool debug_flag = false;

Connection::Connection(const std::string & id, CommClient & client,
                       ServerRouting & svr) : OOGThing(id), m_commClient(client),
                       m_obsolete(false), m_server(svr)
{
    subscribe("login", OP_LOGIN);
    subscribe("logout", OP_LOGOUT);
    subscribe("create", OP_CREATE);
    subscribe("get", OP_GET);
}

Connection::~Connection()
{
    // Once we are obsolete, ExternalMind can no longer affect contents
    // of objects when we delete it.
    assert(!m_obsolete);
    m_obsolete = true;
    ConMap::const_iterator J = m_destroyedConnections.begin();
    ConMap::const_iterator Jend = m_destroyedConnections.end();
    for (; J != Jend; ++J) {
        J->second->disconnect();
        delete J->second;
    }

    debug(std::cout << "destroy called" << std::endl << std::flush;);
    
    BaseDict::const_iterator Iend = m_objects.end();
    for (BaseDict::const_iterator I = m_objects.begin(); I != Iend; ++I) {
        Account * ac = dynamic_cast<Account *>(I->second);
        if (ac != NULL) {
            m_server.m_lobby.delObject(ac);
            ac->m_connection = NULL;
            continue;
        }
        Character * character = dynamic_cast<Character *>(I->second);
        if (character != NULL) {
            if (character->m_externalMind != NULL) {
                delete character->m_externalMind;
                character->m_externalMind = NULL;
            }
        }
    }
}

void Connection::send(const RootOperation & msg) const
{
    m_commClient.send(msg);
}

Account * Connection::addPlayer(const std::string& username,
                                const std::string& password)
{
    std::string hash;
    encrypt_password(password, hash);
    std::string newAccountId;
    Database::instance()->newId(newAccountId);
    Player * player = new Player(this, username, hash, newAccountId);
    addObject(player);
    player->m_connection = this;
    m_server.addAccount(player);
    m_server.m_lobby.addObject(player);
    return player;
}

void Connection::addObject(BaseEntity * obj)
{
    m_objects[obj->getId()] = obj;
    SigC::Connection * con = new SigC::Connection(obj->destroyed.connect(SigC::bind<std::string>(SigC::slot(*this, &Connection::objectDeleted), obj->getId())));
    m_destroyedConnections[obj->getId()] = con;
}

void Connection::removeObject(const std::string & id)
{
    if (!m_obsolete) {
        m_objects.erase(id);
        ConMap::iterator I = m_destroyedConnections.find(id);
        if (I != m_destroyedConnections.end()) {
            delete I->second;
            m_destroyedConnections.erase(I);
        }
    }
}

void Connection::objectDeleted(std::string id) {
    removeObject(id);
}

void Connection::destroy()
{
}

void Connection::disconnect()
{
    m_commClient.disconnect();
}

bool Connection::verifyCredentials(const Account & account,
                                   const MapType & creds) const
{
    MapType::const_iterator I = creds.find("password");
    if ((I == creds.end()) || !I->second.isString()) {
        return false;
    }
    const std::string & passwd = I->second.asString();

    std::string hash;
    encrypt_password(passwd, hash);
    if (hash != account.m_password) {
        return false;
    }
    return true;
}

void Connection::operation(const RootOperation & op, OpVector & res)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    const std::string & from = op.getFrom();
    if (from.empty()) {
        debug(std::cout << "deliver locally" << std::endl << std::flush;);
        callOperation(op, res);
        return;
    } else {
        debug(std::cout << "send on to " << from << std::endl << std::flush;);
        BaseDict::const_iterator I = m_objects.find(from);
        if (I == m_objects.end()) {
            std::string err = "Client \"";
            err += op.getParents().front().asString();
            err += "\" op from \"";
            err += from;
            err += "\" is from non existant object.";
            error(op, err.c_str(), res);
            return;
        }
        BaseEntity * b_ent = I->second;
        Entity * ig_ent = dynamic_cast<Character *>(b_ent);
        if (ig_ent == NULL) {
            b_ent->operation(op, res);
            return;
        }
        Character * character = dynamic_cast<Character *>(b_ent);
        if ((character != NULL) && (character->m_externalMind == NULL)) {
            character->m_externalMind = new ExternalMind(*this,
                                                         character->getId());
            debug(std::cout << "Subscribing existing character" << std::endl
                            << std::flush;);
            Info * info = new Info;
            ListType & info_args = info->getArgs();
            info_args.push_back(MapType());
            character->addToMessage(info_args.back().asMap());
            info->setRefno(op.getSerialno());
            info->setSerialno(m_server.newSerialNo());

            res.push_back(info);
            character->externalOperation(op);
            return;
        }
        ig_ent->externalOperation(op);
        return;
    }
}

void Connection::LoginOperation(const RootOperation & op, OpVector & res)
{

    debug(std::cout << "Got login op" << std::endl << std::flush;);
    if (op.getArgs().empty()) {
        error(op, "Login has no argument", res);
        return;
    }
    if (!op.getArgs().front().isMap()) {
        error(op, "Login arg is malformed", res);
        return;
    }
    // Account should be the first argument of the op
    const MapType & account = op.getArgs().front().asMap();
    // Check for username, and if its not there, then check for
    // id in case we are dealing with an old client.
    MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.isString()) {
        log(WARNING, "Got Login with no username. Checking for old Login");
        I = account.find("id");
        if ((I == account.end()) || !I->second.isString()) {
            error(op, "No username provided for Login", res);
            return;
        }
    }
    const std::string & username = I->second.asString();
    if (username.empty()) {
        error(op, "Empty username provided for Login", res);
        return;
    }
    // We now have username, so can check whether we know this
    // account, either from existing account ....
    Account * player = m_server.getAccountByName(username);
    // or if not, from the database
    if (player == 0) {
        debug(std::cout << "No account called " << username
                        << " in server. Checking in database."
                        << std::endl << std::flush;);
        player = Persistance::instance()->getAccount(username);
        if (player != 0) {
            Persistance::instance()->registerCharacters(*player,
                                               m_server.m_world.getObjects());
            m_server.addAccount(player);
        }
    }
    if ((player == 0) || !verifyCredentials(*player, account)) {
        error(op, "Login is invalid", res);
        return;
    }
    // Account appears to be who they say they are
    if (player->m_connection) {
        // Internals don't allow player to log in more than once.
        error(op, "This account is already logged in", res);
        return;
    }
    // Connect everything up
    addObject(player);
    EntityDict::const_iterator J = player->getCharacters().begin();
    EntityDict::const_iterator Jend = player->getCharacters().end();
    for (; J != Jend; ++J) {
        addObject(J->second);
    }
    player->m_connection = this;
    m_server.m_lobby.addObject(player);
    // Let the client know they have logged in
    Info * info = new Info;
    ListType & info_args = info->getArgs();
    info_args.push_back(MapType());
    player->addToMessage(info_args.front().asMap());
    info->setRefno(op.getSerialno());
    info->setSerialno(m_server.newSerialNo());
    debug(std::cout << "Good login" << std::endl << std::flush;);
    res.push_back(info);
}

void Connection::CreateOperation(const RootOperation & op, OpVector & res)
{
    debug(std::cout << "Got create op" << std::endl << std::flush;);
    if (!m_objects.empty()) {
        error(op, "Already logged in", res);
        return;
    }
    if (op.getArgs().empty()) {
        error(op, "Create has no argument", res);
        return;
    }
    if (!op.getArgs().front().isMap()) {
        error(op, "Create is malformed", res);
        return;
    }
    const MapType & account = op.getArgs().front().asMap();

    if (restricted_flag) {
        error(op, "Account creation on this server is restricted", res);
        return;
    }
    MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.isString()) {
        log(WARNING, "Got Create for account with no username. Checking for old style Create.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.isString()) {
            error(op, "Account creation with no username", res);
            return;
        }
    }

    const std::string & username = I->second.asString();
    I = account.find("password");
    if ((I == account.end()) || !I->second.isString()) {
        error(op, "Account creation with no password", res);
        return;
    }
    const std::string & password = I->second.asString();

    if ((0 != m_server.getAccountByName(username)) ||
        (Persistance::instance()->findAccount(username)) ||
        (username.empty()) || (password.empty())) {
        // Account exists, or creation data is duff
        error(op, "Account creation is invalid", res);
        return;
    }
    Account * player = addPlayer(username, password);
    Persistance::instance()->putAccount(*player);
    Info * info = new Info;
    ListType & info_args = info->getArgs();
    info_args.push_back(MapType());
    player->addToMessage(info_args.front().asMap());
    info->setRefno(op.getSerialno());
    info->setSerialno(m_server.newSerialNo());
    debug(std::cout << "Good create" << std::endl << std::flush;);
    res.push_back(info);
}

void Connection::LogoutOperation(const RootOperation & op, OpVector & res)
{
    if (op.getArgs().empty()) {
        // Logging self out
        Info info;
        ListType & args = info.getArgs();
        args.push_back(op.asObject());
        info.setRefno(op.getSerialno());
        info.setSerialno(m_server.newSerialNo());
        send(info);
        disconnect();
        return;
    }
    if (!op.getArgs().front().isMap()) {
        error(op, "Create arg is not a map", res);
        return;
    }
    const MapType & account = op.getArgs().front().asMap();
    
    MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.isString()) {
        log(WARNING, "Got Logout with no username. Checking for old style Logout.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.isString()) {
            error(op, "Logout is invalid", res);
            return;
        }
    }
    const std::string & username = I->second.asString();
    I = account.find("password");
    if ((I == account.end()) || (!I->second.isString())) {
        error(op, "No account password given", res);
        return;
    }
    const std::string & password = I->second.asString();
    Account * player = m_server.getAccountByName(username);
    if ((!player) || (password != player->m_password)) {
        error(op, "Logout failed", res);
        return;
    }

    // FIXME This won't work. This connection won't have the account ID
    // so won't be able to find it. If it did, then it could just log out
    // the normal way. Pointless.
    RootOperation l(op);
    l.setFrom(player->getId());

    OpVector tres;
    operation(l, tres);
    // Its not safe to assert this. FIXME Look into what might be returned
    // in the way of errors, and where they should go.
    assert(tres.empty());
}

void Connection::GetOperation(const RootOperation & op, OpVector & res)
{
    const ListType & args = op.getArgs();

    Info * info;
    if (args.empty() || !args.front().isMap()) {
        info = new Info;
        ListType & info_args = info->getArgs();
        info_args.push_back(MapType());
        m_server.addToMessage(info_args.front().asMap());
        info->setRefno(op.getSerialno());
        info->setSerialno(m_server.newSerialNo());
        debug(std::cout << "Replying to empty get" << std::endl << std::flush;);
    } else {
        MapType::const_iterator I = args.front().asMap().find("id");
        if ((I == args.front().asMap().end()) || (!I->second.isString())) {
            error(op, "Type definition requested with no id", res);
            return;
        }
        const std::string & id = I->second.asString();
        debug(std::cout << "Get got for " << id << std::endl << std::flush;);
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o == NULL) {
            std::string msg("Unknown type definition for \"");
            msg += id;
            msg += "\" requested";
            error(op, msg.c_str(), res);
            return;
        }
        info = new Info;
        ListType & iargs = info->getArgs();
        iargs.push_back(o->asObject());
        info->setRefno(op.getSerialno());
        info->setSerialno(m_server.newSerialNo());
    }
    
    res.push_back(info);
}
