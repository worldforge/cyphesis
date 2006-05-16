// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2006 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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

Connection::Connection(CommClient & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id) :
            OOGThing(id, forceIntegerId(id)), m_obsolete(false),
                                              m_commClient(client),
                                              m_server(svr)
{
    m_server.incClients();
    logEvent(CONNECT, String::compose("%1 - - Connect from %2", id, addr).c_str());
}

Connection::~Connection()
{
    // Once we are obsolete, ExternalMind can no longer affect contents
    // of objects when we delete it.
    assert(!m_obsolete);
    m_obsolete = true;

    debug(std::cout << "destroy called" << std::endl << std::flush;);
    
    logEvent(DISCONNECT, String::compose("%1 - - Disconnect", getId()).c_str());

    BaseDict::const_iterator Iend = m_objects.end();
    for (BaseDict::const_iterator I = m_objects.begin(); I != Iend; ++I) {
        Account * ac = dynamic_cast<Account *>(I->second);
        if (ac != NULL) {
            m_server.m_lobby.delAccount(ac);
            ac->m_connection = NULL;
            logEvent(LOGOUT, String::compose("%1 %2 - Disconnect account %3", getId(), ac->getId(), ac->m_username).c_str());
            continue;
        }
        Character * character = dynamic_cast<Character *>(I->second);
        if (character != NULL) {
            if (character->m_externalMind != NULL) {
                delete character->m_externalMind;
                character->m_externalMind = NULL;
                logEvent(DROP_CHAR, String::compose("%1 - %2 Disconnect character %3(%4)", getId(), character->getId(), character->getName(), character->getType()).c_str());
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
    if (intId < 0) {
        log(ERROR, "Account creation failed as no ID available");
        return 0;
    }

    Player * player = new Player(this, username, hash, newAccountId, intId);
    addObject(player);
    assert(player->m_connection == this);
    player->m_connection = this;
    m_server.addAccount(player);
    m_server.m_lobby.addAccount(player);
    return player;
}

void Connection::addObject(BaseEntity * obj)
{
    m_objects[obj->getIntId()] = obj;
    obj->destroyed.connect(sigc::bind(sigc::mem_fun(this, &Connection::objectDeleted), obj->getIntId()));
}

void Connection::removeObject(long id)
{
    if (!m_obsolete) {
        m_objects.erase(id);
    }
}

void Connection::objectDeleted(long id)
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
    }
    const std::string & from = op->getFrom();
    debug(std::cout << "send on to " << from << std::endl << std::flush;);
    BaseDict::const_iterator I = m_objects.find(integerId(from));
    if (I == m_objects.end()) {
        error(op, String::compose("Client \"%1\" op from \"%2\" is from non-existant object.", op->getParents().front(), from).c_str(), res);
        return;
    }
    BaseEntity * b_ent = I->second;
    Entity * ig_ent = dynamic_cast<Entity *>(b_ent);
    if (ig_ent == NULL) {
        b_ent->operation(op, res);
        return;
    }
    Character * character = dynamic_cast<Character *>(b_ent);
    if ((character != NULL) && (character->m_externalMind == NULL)) {
        debug(std::cout << "Subscribing existing character" << std::endl
                        << std::flush;);
        character->m_externalMind = new ExternalMind(*this, character->getId(),
                                                     character->getIntId());
        Info info;
        Anonymous info_arg;
        character->addToEntity(info_arg);
        info->setArgs1(info_arg);
        info->setSerialno(newSerialNo());

        res.push_back(info);

        logEvent(TAKE_CHAR, String::compose("%1 - %2 Taken character %3(%4)", getId(), ig_ent->getId(), ig_ent->getName(), ig_ent->getType()).c_str());
    }
    ig_ent->externalOperation(op);
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
    m_server.m_lobby.addAccount(player);
    // Let the client know they have logged in
    Info info;
    Anonymous info_arg;
    player->addToEntity(info_arg);
    info->setArgs1(info_arg);
    info->setSerialno(newSerialNo());
    debug(std::cout << "Good login" << std::endl << std::flush;);
    res.push_back(info);

    logEvent(LOGIN, String::compose("%1 %2 - Login account %3", getId(), player->getId(), username).c_str());
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
    if (player == 0) {
        clientError(op, "Account creation failed", res);
        return;
    }
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

    logEvent(LOGIN, String::compose("%1 %2 - Create account %3", getId(), player->getId(), username).c_str());
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
    
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Got logout for entith with no ID", res);
        return;
    }
    const long obj_id = integerId(arg->getId());
    if (obj_id == -1) {
        error(op, "Got logout for non numeric entity ID", res);
        return;
    }
    BaseDict::iterator I = m_objects.find(obj_id);
    if (I == m_objects.end()) {
        error(op, "Got logout for unknown entity ID", res);
        return;
    }
    // This is very similar code to stuff that does the same job in
    // Connection destructor. Should be possible to share some, but
    // the log messages should probably be different.
    Account * ac = dynamic_cast<Account *>(I->second);
    Character * character = dynamic_cast<Character *>(I->second);
    if (ac != 0) {
        m_server.m_lobby.delAccount(ac);
        ac->m_connection = 0;
        m_objects.erase(I);
        // FIXME Remove all characters associated with this account.
        logEvent(LOGOUT, String::compose("%1 %2 - Logout account %3", getId(), ac->getId(), ac->m_username).c_str());
    }
    if (character != 0) {
        std::cout << "Loogging out character" << std::endl << std::flush;
        assert(character->m_externalMind != 0);
        delete character->m_externalMind;
        character->m_externalMind = 0;
        logEvent(DROP_CHAR, String::compose("%1 - %2 Logout character %3(%4)", getId(), character->getId(), character->getName(), character->getType()).c_str());

    }

    Info info;
    info->setArgs1(op);
    info->setRefno(op->getSerialno());
    // Err, do we still bother with this?
    info->setSerialno(newSerialNo());
    res.push_back(info);
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
