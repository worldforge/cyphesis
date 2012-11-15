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

// $Id$

#include "Connection.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommServer.h"
#include "Player.h"
#include "ExternalMind.h"
#include "ExternalProperty.h"

#include "rulesets/Character.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/Update.h"
#include "common/globals.h"
#include "common/serialno.h"
#include "common/Inheritance.h"
#include "common/system.h"
#include "common/TypeNode.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

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

Connection::Connection(CommSocket & socket,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Link(socket, id, iid), m_obsolete(false),
                                                m_server(svr)
{
    m_server.incClients();
    logEvent(CONNECT, String::compose("%1 - - Connect from %2", id, addr));
}

Connection::~Connection()
{
    // Once we are obsolete, ExternalMind can no longer affect contents
    // of objects when we delete it.
    assert(!m_obsolete);
    m_obsolete = true;

    debug(std::cout << "destroy called" << std::endl << std::flush;);
    
    logEvent(DISCONNECT, String::compose("%1 - - Disconnect", getId()));

    RouterMap::iterator Iend = m_objects.end();
    for (RouterMap::iterator I = m_objects.begin(); I != Iend; ++I) {
        disconnectObject(I, "Disconnect");
    }

    m_server.decClients();
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & hash,
                                 const std::string & id, long intId)
{
    return new Player(this, username, hash, id, intId);
}

static const int hash_salt_size = 8;

Account * Connection::addNewAccount(const std::string & type,
                                    const std::string & username,
                                    const std::string & password)
{
    std::string hash;
    std::string salt = m_server.getShaker().generateSalt(hash_salt_size);
    hash_password(password,salt,hash);
    std::string newAccountId;

    long intId = newId(newAccountId);
    if (intId < 0) {
        log(ERROR, "Account creation failed as no ID available");
        return 0;
    }

    Account * account = newAccount(type, username, hash, newAccountId, intId);
    if (account == 0) {
        return 0;
    }
    addObject(account);
    assert(account->m_connection == this);
    account->m_connection = this;
    m_server.addAccount(account);
    m_server.m_lobby.addAccount(account);
    return account;
}

/// \brief Remove an object from this connection.
///
/// The object being removed may be a player, or another type of object such
/// as an avatar. If it is an player or other account, a pointer is returned.
void Connection::disconnectObject(RouterMap::iterator I,
                                  const std::string & event)
{
    ConnectableRouter * cr = dynamic_cast<ConnectableRouter *>(I->second);
    if (cr != 0) {
        cr->m_connection = 0;
        Account * ac = dynamic_cast<Account *>(cr);
        if (ac != 0) {
            disconnectAccount(ac, I, event);
        }
        return;
    }
    Character * chr = dynamic_cast<Character *>(I->second);
    if (chr != 0) {
        if (chr->m_externalMind != 0) {
            ExternalMind * mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
            if (mind != 0 && mind->isLinkedTo(this)) {
                // Send a move op stopping the current movement
                Anonymous move_arg;
                move_arg->setId(chr->getId());
                // Include the EXTERNAL property which is changing to zero.
                // It would be more correct at this point to send a separate
                // update to have the property update itself, but this
                // will be much less of an issue once Sight(Set) is created
                // more correctly
                move_arg->setAttr("external", 0);
                ::addToEntity(Vector3D(0,0,0), move_arg->modifyVelocity());

                Move move;
                move->setFrom(chr->getId());
                move->setArgs1(move_arg);
                chr->externalOperation(move);

                // We used to delete the external mind here, but now we
                // leave it in place, as it takes care of the disconnected
                // character.
                mind->linkUp(0);
                logEvent(DROP_CHAR, String::compose("%1 - %2 %4 character (%3)",
                                                    getId(), chr->getId(),
                                                    chr->getType()->name(),
                                                    event));
            } else if (mind != 0 && mind->isLinked()) {
                log(ERROR, String::compose("Connection(%1) requested to "
                                           "remove active character %2(%3) "
                                           "which is subscribed to another "
                                           "Connection(%4).", getId(),
                                           chr->getType()->name(),
                                           chr->getId(),
                                           mind->connectionId()));
            }
        }
    }
    return;
}

void Connection::addEntity(Entity * ent)
{
    addObject(ent);
    ent->destroyed.connect(sigc::bind(sigc::mem_fun(this,
                                                    &Connection::objectDeleted),
                                      ent->getIntId()));
}

void Connection::addObject(Router * obj)
{
    m_objects[obj->getIntId()] = obj;
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

void Connection::connectAvatar(Character * chr)
{
    ExternalMind * mind = 0;
    if (chr->m_externalMind != 0) {
        mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
    }
    if (mind == 0) {
        chr->m_externalMind = mind = new ExternalMind(*chr);
    }

    if (mind->isLinked()) {
        log(ERROR, "Character is already connected.");
        return;
    }
    mind->linkUp(this);

    if (chr->getProperty("external") == 0) {
        ExternalProperty * ep = new ExternalProperty(chr->m_externalMind);
        // FIXME ensure this is install()ed and apply()ed
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

    return check_password(passwd, account.password());
}

void Connection::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    if (!op->hasAttrFlag(Atlas::Objects::Operation::FROM_FLAG)) {
        debug(std::cout << "deliver locally" << std::endl << std::flush;);
        const OpNo op_no = op->getClassNo();
        switch (op_no) {
            case Atlas::Objects::Operation::CREATE_NO:
                CreateOperation(op, res);
                break;
            case Atlas::Objects::Operation::GET_NO:
                GetOperation(op, res);
                break;
            case Atlas::Objects::Operation::LOGIN_NO:
                LoginOperation(op, res);
                break;
            case Atlas::Objects::Operation::LOGOUT_NO:
                LogoutOperation(op, res);
                break;
            case OP_INVALID:
                break;
            default:
                error(op, "Unknown operation", res);
                break;
        }
        return;
    }
    const std::string & from = op->getFrom();
    debug(std::cout << "send on to " << from << std::endl << std::flush;);
    RouterMap::const_iterator I = m_objects.find(integerId(from));
    if (I == m_objects.end()) {
        error(op, String::compose("Client \"%1\" op from \"%2\" is from "
                                  "non-existant object.",
                                  op->getParents().front(), from),
              res);
        return;
    }
    Router * obj = I->second;
    Entity * ig_ent = dynamic_cast<Entity *>(obj);
    if (ig_ent == NULL) {
        obj->operation(op, res);
        return;
    }
    Character * chr = dynamic_cast<Character *>(obj);
    if (chr != NULL) {
        ExternalMind * mind = 0;
        if (chr->m_externalMind != 0) {
            mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
        }
        if (mind == 0 || !mind->isLinked()) {
            debug(std::cout << "Subscribing existing character" << std::endl
                            << std::flush;);
            connectAvatar(chr);
            Info info;
            Anonymous info_arg;
            chr->addToEntity(info_arg);
            info->setArgs1(info_arg);

            res.push_back(info);

            logEvent(TAKE_CHAR, String::compose("%1 - %2 Taken character (%3)",
                                                getId(), ig_ent->getId(),
                                                ig_ent->getType()));
        }
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
    Account * account = m_server.getAccountByName(username);
    // or if not, from the database
    if (account == 0 || verifyCredentials(*account, arg) != 0) {
        clientError(op, "Login is invalid", res);
        return;
    }
    // Account appears to be who they say they are
    if (account->m_connection) {
        // Internals don't allow player to log in more than once.
        clientError(op, "This account is already logged in", res);
        return;
    }
    // Connect everything up
    addObject(account);
    EntityDict::const_iterator J = account->getCharacters().begin();
    EntityDict::const_iterator Jend = account->getCharacters().end();
    for (; J != Jend; ++J) {
        addEntity(J->second);
    }
    account->m_connection = this;
    m_server.m_lobby.addAccount(account);
    // Let the client know they have logged in
    Info info;
    Anonymous info_arg;
    account->addToEntity(info_arg);
    info->setArgs1(info_arg);
    debug(std::cout << "Good login" << std::endl << std::flush;);
    res.push_back(info);

    logEvent(LOGIN, String::compose("%1 %2 - Login account %3 (%4)",
                                    getId(), account->getId(), username,
                                    account->getType()));
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
        error(op, "Got account Create with no username.", res);
        return;
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
        (0 != m_server.getAccountByName(username))) {
        // Account exists, or creation data is duff
        clientError(op, "Account creation is invalid", res);
        return;
    }
    std::string type("player");
    if (!arg->isDefaultParents()) {
        const std::list<std::string> & parents = arg->getParents();
        if (!parents.empty()) {
            type = parents.front();
        }
    }
    Account * account = addNewAccount(type, username, password);
    if (account == 0) {
        clientError(op, "Account creation failed", res);
        return;
    }
    Info info;
    Anonymous info_arg;
    account->addToEntity(info_arg);
    info->setArgs1(info_arg);
    debug(std::cout << "Good create" << std::endl << std::flush;);
    res.push_back(info);

    logEvent(LOGIN, String::compose("%1 %2 - Create account %3 (%4)",
                                    getId(),
                                    account->getId(),
                                    username,
                                    account->getType()));
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
    RouterMap::iterator I = m_objects.find(obj_id);
    if (I == m_objects.end()) {
        error(op, String::compose("Got logout for unknown entity ID(%1)",
                                  obj_id),
              res);
        return;
    }
    disconnectObject(I, "Logout");

    Info info;
    info->setArgs1(op);
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }
    res.push_back(info);
}

void Connection::disconnectAccount(Account * ac,
                                   RouterMap::iterator I,
                                   const std::string & event)
{
    m_server.m_lobby.delAccount(ac);
    logEvent(LOGOUT, String::compose("%1 %2 - %4 account %3", getId(),
                                     ac->getId(), ac->username(),
                                     event));
    if (m_obsolete) {
        return;
    }
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
                    log(ERROR, String::compose("Character %1(%2) has "
                                               "external mind object which "
                                               "is not an ExternalMind",
                                               chr->getType(),
                                               chr->getId()));
                } else {
                    if (!em->isLinked()) {
                        log(ERROR,
                            String::compose("Connection(%1) has found a "
                                            "character in its dictionery "
                                            "which is not connected.",
                                            getId()));
                        removeObject(chr->getIntId());
                    } else if (!em->isLinkedTo(this)) {
                        log(ERROR,
                            String::compose("Connection(%1) has found a "
                                            "character in its dictionery "
                                            "which is connected to another "
                                            "Connection(%2)", getId(),
                                            em->connectionId()));
                        removeObject(chr->getIntId());
                    }
                }
            } else {
                removeObject(chr->getIntId());
            }
        } else {
            // Non character entity
            removeObject(chr->getIntId());
        }
    }
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
            error(op, String::compose("Unknown type definition for \"%1\" "
                                      "requested", id),
                  res);
            return;
        }
        info->setArgs1(o);
    }
    
    res.push_back(info);
}
