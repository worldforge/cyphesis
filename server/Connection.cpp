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

// $Id: Connection.cpp,v 1.164 2007-02-20 00:52:43 alriddoch Exp $

#include "Connection.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommServer.h"
#include "Player.h"
#include "ExternalMind.h"
#include "Persistance.h"
#include "ExternalProperty.h"

#include "rulesets/Character.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/Update.h"
#include "common/globals.h"
#include "common/serialno.h"
#include "common/inheritance.h"
#include "common/system.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Update;
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
        removePlayer(I->second, "Disconnect");
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

/// \brief Remove an object from this connection.
///
/// The object being removed may be a player, or another type of object such
/// as an avatar. If it is an player or other account, a pointer is returned.
Account * Connection::removePlayer(BaseEntity * obj, const std::string & event)
{
    Account * ac = dynamic_cast<Account *>(obj);
    if (ac != 0) {
        m_server.m_lobby.delAccount(ac);
        ac->m_connection = 0;
        logEvent(LOGOUT, String::compose("%1 %2 - %4 account %3", getId(), ac->getId(), ac->m_username, event).c_str());
        return ac;
    }
    Character * character = dynamic_cast<Character *>(obj);
    if (character != 0) {
        if (character->m_externalMind != 0) {
            // Send a move op stopping the current movement
            Anonymous move_arg;
            move_arg->setId(character->getId());
            // Include the EXTERNAL property which is changing to zero.
            // It would be more correct at this point to send a separate
            // update to have the property update itself, but this
            // will be much less of an issue once Sight(Set) is created
            // more correctly
            move_arg->setAttr("external", 0);
            ::addToEntity(Vector3D(0,0,0), move_arg->modifyVelocity());

            Move move;
            move->setFrom(character->getId());
            move->setArgs1(move_arg);
            character->externalOperation(move);

            delete character->m_externalMind;
            character->m_externalMind = 0;
            logEvent(DROP_CHAR, String::compose("%1 - %2 %5 character %3(%4)", getId(), character->getId(), character->getName(), character->getType(), event).c_str());
        }
    }
    return 0;
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

void Connection::connectAvatar(Character * chr)
{
    chr->m_externalMind = new ExternalMind(*this, chr->getId(),
                                                  chr->getIntId());

    if (chr->getProperty("external") == 0) {
        ExternalProperty * ep = new ExternalProperty(chr->m_externalMind);
        chr->setProperty("external", ep);
    }

    Anonymous update_arg;
    update_arg->setId(chr->getId());
    update_arg->setAttr("external", 1);

    Update update;
    update->setTo(chr->getId());
    update->setArgs1(update_arg);

    chr->sendWorld(update);
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
        connectAvatar(character);
        Info info;
        Anonymous info_arg;
        character->addToEntity(info_arg);
        info->setArgs1(info_arg);

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
    if (database_flag && player == 0) {
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

    if (username.empty() || password.empty() ||
        (0 != m_server.getAccountByName(username)) ||
        (database_flag && Persistance::instance()->findAccount(username))) {
        // Account exists, or creation data is duff
        clientError(op, "Account creation is invalid", res);
        return;
    }
    Account * player = addPlayer(username, password);
    if (player == 0) {
        clientError(op, "Account creation failed", res);
        return;
    }
    if (database_flag) {
        Persistance::instance()->putAccount(*player);
    }
    Info info;
    Anonymous info_arg;
    player->addToEntity(info_arg);
    info->setArgs1(info_arg);
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
        if (!op->isDefaultSerialno()) {
            info->setRefno(op->getSerialno());
        }
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
        error(op, String::compose("Got logout for unknown entity ID(%1)", obj_id).c_str(), res);
        return;
    }
    Account * ac = removePlayer(I->second, "Logout");
    if (ac != 0) {
        m_objects.erase(I);
        EntityDict::const_iterator J = ac->getCharacters().begin();
        EntityDict::const_iterator Jend = ac->getCharacters().end();
        for (; J != Jend; ++J) {
            Entity * chr = J->second;
            Character * character = dynamic_cast<Character *>(chr);
            if (character != 0) {
                if (character->m_externalMind != 0) {
                    ExternalMind * em = dynamic_cast<ExternalMind *>(character->m_externalMind);
                    if (em == 0) {
                        log(ERROR, String::compose("Character %1(%2) has external mind object which is not an ExternalMind", chr->getType(), chr->getId()).c_str());
                    } else {
                        if (&em->m_connection != this) {
                            log(ERROR, String::compose("Connection(%1) has found a character in its dictionery which is connected to another Connection(%2)", getId(), em->m_connection.getId()).c_str());
                            removeObject(J->second->getIntId());
                        }
                    }
                } else {
                    removeObject(J->second->getIntId());
                }
            } else {
                // Non character entity
                removeObject(J->second->getIntId());
            }
        }
    }

    Info info;
    info->setArgs1(op);
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }
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
    }
    
    res.push_back(info);
}
