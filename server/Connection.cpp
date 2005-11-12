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

#include "common/id.h"
#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/serialno.h"
#include "common/inheritance.h"
#include "common/system.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Connection::Connection(const std::string & id, CommClient & client,
                       ServerRouting & svr) : OOGThing(id, -1),
                       m_obsolete(false), m_commClient(client), m_server(svr)
{
    m_server.incClients();
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

    m_server.decClients();
}

void Connection::send(const Operation & op) const
{
    m_commClient.send(op);
}

Account * Connection::addPlayer(const std::string& username,
                                const std::string& password)
{
    std::string hash;
    encrypt_password(password, hash);
    std::string newAccountId;

    long intId = newId(newAccountId);
    assert(!newAccountId.empty());

    Player * player = new Player(this, username, hash, newAccountId, intId);
    addObject(player);
    assert(player->m_connection == this);
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

void Connection::objectDeleted(std::string id)
{
    removeObject(id);
}

void Connection::disconnect()
{
    m_commClient.disconnect();
}

int Connection::verifyCredentials(const Account & account,
                                  const Root & creds) const
{
    Element passwd_attr;
    if (creds->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        return -1;
    }
    const std::string & passwd = passwd_attr.String();

    return check_password(passwd, account.m_password);
}

void Connection::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    if (!op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG)) {
        debug(std::cout << "deliver locally" << std::endl << std::flush;);
        callOperation(op, res);
        return;
    } else {
        const std::string & from = op->getFrom();
        debug(std::cout << "send on to " << from << std::endl << std::flush;);
        BaseDict::const_iterator I = m_objects.find(from);
        if (I == m_objects.end()) {
            std::string err = "Client \"";
            err += op->getParents().front();
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
                                                         character->getId(),
                                                         character->getIntId());
            debug(std::cout << "Subscribing existing character" << std::endl
                            << std::flush;);
            Info info;
            Anonymous info_arg;
            character->addToEntity(info_arg);
            info->setArgs1(info_arg);
            info->setSerialno(newSerialNo());

            res.push_back(info);
            character->externalOperation(op);
            return;
        }
        ig_ent->externalOperation(op);
        return;
    }
}

void Connection::LoginOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got login op" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Login has no argument", res);
        return;
    }
    // Account should be the first argument of the op
    const Root & arg = args.front();
    // Check for username, and if its not there, then check for
    // id in case we are dealing with an old client.
    Element user_attr;
    std::string username;
    if (arg->copyAttr("username", user_attr) != 0 || !user_attr.isString()) {
        log(WARNING, "Got Login for account with no username. Checking for old style Login.");
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Got account Login with no username.", res);
            return;
        }
        username = arg->getId();
    } else {
        username = user_attr.String();
    }
    if (username.empty()) {
        error(op, "Empty username provided for Login", res);
        return;
    }

    // We now have username, so can check whether we know this
    // account, either from existing account ....
    Account * player = m_server.getAccountByName(username);
    // or if not, from the database
    if (consts::enable_database && player == 0) {
        debug(std::cout << "No account called " << username
                        << " in server. Checking in database."
                        << std::endl << std::flush;);
        player = Persistance::instance()->getAccount(username);
        if (player != 0) {
            Persistance::instance()->registerCharacters(*player,
                                               m_server.m_world.getEntities());
            m_server.addAccount(player);
        }
    }
    if (player == 0 || verifyCredentials(*player, arg) != 0) {
        clientError(op, "Login is invalid", res);
        return;
    }
    // Account appears to be who they say they are
    if (player->m_connection) {
        // Internals don't allow player to log in more than once.
        clientError(op, "This account is already logged in", res);
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
    Info info;
    Anonymous info_arg;
    player->addToEntity(info_arg);
    info->setArgs1(info_arg);
    info->setSerialno(newSerialNo());
    debug(std::cout << "Good login" << std::endl << std::flush;);
    res.push_back(info);
}

void Connection::CreateOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got create op" << std::endl << std::flush;);
    if (!m_objects.empty()) {
        clientError(op, "This connection is already logged in", res);
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Create has no argument", res);
        return;
    }
    const Root & arg = args.front();

    if (restricted_flag) {
        error(op, "Account creation on this server is restricted", res);
        return;
    }

    Element user_attr;
    std::string username;
    if (arg->copyAttr("username", user_attr) != 0 || !user_attr.isString()) {
        log(WARNING, "Got Create for account with no username. Checking for old style Create.");
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Got account Create with no username.", res);
            return;
        }
        username = arg->getId();
    } else {
        username = user_attr.String();
    }

    Element passwd_attr;
    if (arg->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        error(op, "No account password given", res);
        return;
    }
    const std::string & password = passwd_attr.String();

    if ((0 != m_server.getAccountByName(username)) ||
        (consts::enable_database && Persistance::instance()->findAccount(username)) ||
        (username.empty()) || (password.empty())) {
        // Account exists, or creation data is duff
        clientError(op, "Account creation is invalid", res);
        return;
    }
    Account * player = addPlayer(username, password);
    if (consts::enable_database) {
        Persistance::instance()->putAccount(*player);
    }
    Info info;
    Anonymous info_arg;
    player->addToEntity(info_arg);
    info->setArgs1(info_arg);
    info->setSerialno(newSerialNo());
    debug(std::cout << "Good create" << std::endl << std::flush;);
    res.push_back(info);
}

void Connection::LogoutOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        // Logging self out
        Info info;
        info->setArgs1(op);
        info->setRefno(op->getSerialno());
        info->setSerialno(newSerialNo());
        // FIXME Direct call of send. Need local refno handling.
        send(info);
        disconnect();
        return;
    }
    const Root & arg = args.front();
    
    Element user_attr;
    if (arg->copyAttr("username", user_attr) != 0 || !user_attr.isString()) {
        error(op, "Got Logout with no username.", res);
        return;
    }
    const std::string & username = user_attr.String();

    Element passwd_attr;
    if (arg->copyAttr("password", passwd_attr) != 0 || !passwd_attr.isString()) {
        error(op, "No account password given", res);
        return;
    }
    const std::string & password = passwd_attr.String();
    Account * player = m_server.getAccountByName(username);
    if ((!player) || (password != player->m_password)) {
        error(op, "Logout failed", res);
        return;
    }

    // FIXME This won't work. This connection won't have the account ID
    // so won't be able to find it. If it did, then it could just log out
    // the normal way. Pointless.
    Operation l(op.copy());
    l->setFrom(player->getId());

    OpVector tres;
    operation(l, tres);
    // Its not safe to assert this. FIXME Look into what might be returned
    // in the way of errors, and where they should go.
    assert(tres.empty());
}

void Connection::GetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();

    Info info;
    if (args.empty()) {
        Anonymous info_arg;
        m_server.addToEntity(info_arg);
        info->setArgs1(info_arg);
        info->setSerialno(newSerialNo());
        debug(std::cout << "Replying to empty get" << std::endl << std::flush;);
    } else {
        const Root & arg = args.front();
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Type definition requested with no id", res);
            return;
        }
        const std::string & id = arg->getId();
        debug(std::cout << "Get got for " << id << std::endl << std::flush;);
        Atlas::Objects::Root o = Inheritance::instance().getClass(id);
        if (!o.isValid()) {
            error(op, String::compose("Unknown type definition for \"%1\" requested", id).c_str(), res);
            return;
        }
        info->setArgs1(o);
        info->setSerialno(newSerialNo());
    }
    
    res.push_back(info);
}
