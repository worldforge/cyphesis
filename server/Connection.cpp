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
    // Once we are obsolete, ExternalMind can no longer affect contents
    // of objects when we delete it.
    assert(!m_obsolete);
    m_obsolete = true;
    ConMap::const_iterator J = m_destroyedConnections.begin();
    for(; J != m_destroyedConnections.end(); J++) {
        J->second->disconnect();
        delete J->second;
    }

    debug(std::cout << "destroy called" << std::endl << std::flush;);
    BaseDict::const_iterator I;
    for(I = m_objects.begin(); I != m_objects.end(); I++) {
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

void Connection::close()
{
    m_commClient.close();
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

OpVector Connection::operation(const RootOperation & op)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    const std::string & from = op.getFrom();
    if (from.empty()) {
        debug(std::cout << "deliver locally" << std::endl << std::flush;);
        return callOperation(op);
    } else {
        debug(std::cout << "send on to " << from << std::endl << std::flush;);
        BaseDict::const_iterator I = m_objects.find(from);
        if (I == m_objects.end()) {
            std::string err = "Client \"";
            err += op.getParents().front().asString();
            err += "\" op from \"";
            err += from;
            err += "\" is from non existant object.";
            return error(op, err.c_str());
        }
        BaseEntity * ent = I->second;
        Character * character = dynamic_cast<Character *>(ent);
        if ((character != NULL) && (character->m_externalMind == NULL)) {
            character->m_externalMind = new ExternalMind(*this,
                       character->getId(), character->getName());
            debug(std::cout << "Subscribing existing character" << std::endl
                            << std::flush;);
            Info * info = new Info;
            ListType & info_args = info->getArgs();
            info_args.push_back(MapType());
            character->addToMessage(info_args.front().asMap());
            info->setRefno(op.getSerialno());
            info->setSerialno(m_server.newSerialNo());
            OpVector res = ent->externalOperation(op);
            res.insert(res.begin(), info);
            return res;
        }
        return ent->externalOperation(op);
    }
    return OpVector();
}

OpVector Connection::LoginOperation(const Login & op)
{

    debug(std::cout << "Got login op" << std::endl << std::flush;);
    if (op.getArgs().empty()) {
        return error(op, "Login has no argument");
    }
    if (!op.getArgs().front().isMap()) {
        return error(op, "Login arg is malformed");
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
            return error(op, "No username provided for Login");
        }
    }
    const std::string & username = I->second.asString();
    if (username.empty()) {
        return error(op, "Empty username provided for Login");
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
        return error(op, "Login is invalid");
    }
    // Account appears to be who they say they are
    if (player->m_connection) {
        // Internals don't allow player to log in more than once.
        return error(op, "This account is already logged in");
    }
    // Connect everything up
    addObject(player);
    EntityDict::const_iterator J = player->getCharacters().begin();
    for (; J != player->getCharacters().end(); J++) {
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
    return OpVector(1,info);
}

OpVector Connection::CreateOperation(const Create & op)
{
    debug(std::cout << "Got create op" << std::endl << std::flush;);
    if (!m_objects.empty()) {
        return error(op, "Already logged in");
    }
    if (op.getArgs().empty()) {
        return error(op, "Create has no argument");
    }
    if (!op.getArgs().front().isMap()) {
        return error(op, "Create is malformed");
    }
    const MapType & account = op.getArgs().front().asMap();

    if (restricted_flag) {
        return error(op, "Account creation on this server is restricted");
    }
    MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.isString()) {
        log(WARNING, "Got Create for account with no username. Checking for old style Create.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.isString()) {
            return error(op, "Account creation with no username");
        }
    }

    const std::string & username = I->second.asString();
    I = account.find("password");
    if ((I == account.end()) || !I->second.isString()) {
        return error(op, "Account creation with no password");
    }
    const std::string & password = I->second.asString();

    if ((0 != m_server.getAccountByName(username)) ||
        (Persistance::instance()->findAccount(username)) ||
        (username.empty()) || (password.empty())) {
        // Account exists, or creation data is duff
        return error(op, "Account creation is invalid");
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
    return OpVector(1,info);
}

OpVector Connection::LogoutOperation(const Logout & op)
{
    if (op.getArgs().empty()) {
        // Logging self out
        Info info;
        ListType & args = info.getArgs();
        args.push_back(op.asObject());
        info.setRefno(op.getSerialno());
        info.setSerialno(m_server.newSerialNo());
        send(info);
        close();
        return OpVector();
    }
    if (!op.getArgs().front().isMap()) {
        return error(op, "Create arg is not a map");
    }
    const MapType & account = op.getArgs().front().asMap();
    
    MapType::const_iterator I = account.find("username");
    if ((I == account.end()) || !I->second.isString()) {
        log(WARNING, "Got Logout with no username. Checking for old style Logout.");
        I = account.find("id");
        if ((I == account.end()) || !I->second.isString()) {
            return error(op, "Logout is invalid");
        }
    }
    const std::string & username = I->second.asString();
    I = account.find("password");
    if ((I == account.end()) || (!I->second.isString())) {
        return error(op, "No account password given");
    }
    const std::string & password = I->second.asString();
    Account * player = m_server.getAccountByName(username);
    if ((!player) || (password != player->m_password)) {
        return error(op, "Logout failed");
    }
    Logout l = op;
    l.setFrom(player->getId());
    operation(l);

    return OpVector();
}

OpVector Connection::GetOperation(const Get & op)
{
    const ListType & args = op.getArgs();

    Info * info;
    if (args.empty()) {
        info = new Info;
        ListType & info_args = info->getArgs();
        info_args.push_back(MapType());
        m_server.addToMessage(info_args.front().asMap());
        info->setRefno(op.getSerialno());
        info->setSerialno(m_server.newSerialNo());
        debug(std::cout << "Replying to empty get" << std::endl << std::flush;);
    } else {
        if (!args.front().isMap()) {
            return error(op, "Get op arg is not a map");
        }
        MapType::const_iterator I = args.front().asMap().find("id");
        if ((I == args.front().asMap().end()) || (!I->second.isString())) {
            return error(op, "Type definition requested with no id");
        }
        const std::string & id = I->second.asString();
        debug(std::cout << "Get got for " << id << std::endl << std::flush;);
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o == NULL) {
            std::string msg("Unknown type definition for \"");
            msg += id;
            msg += "\" requested";
            return error(op, msg.c_str());
        }
        info = new Info;
        ListType & iargs = info->getArgs();
        iargs.push_back(o->asObject());
        info->setRefno(op.getSerialno());
        info->setSerialno(m_server.newSerialNo());
    }
    
    return OpVector(1,info);
}
