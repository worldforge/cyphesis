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
#include "Player.h"
#include "ExternalMindsConnection.h"
#include "ExternalMindsManager.h"

#include "rulesets/Character.h"
#include "rulesets/ExternalMind.h"

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
#include <algorithm>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Entity::Anonymous;

using String::compose;

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

    //It's important that we disconnect ourselves as a possession router before we disconnect our objects,
    //else there's a risk that the external minds manager will just issue new possession request to this connection.
    for (auto& routerId : m_possessionRouters) {
        ExternalMindsManager::instance()->removeConnection(routerId);
    }

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
        // FIXME assert that the connection pointer points to this
        cr->m_connection = 0;
        Account * ac = dynamic_cast<Account *>(cr);
        if (ac != 0) {
            disconnectAccount(ac, I, event);
        }
        return;
    }
    Character * chr = dynamic_cast<Character *>(I->second);
    if (chr != 0) {
        int taken = chr->unlinkExternal(this);
        if (taken == 0) {
            logEvent(DROP_CHAR, String::compose("%1 - %2 %4 character (%3)",
                                                getId(), chr->getId(),
                                                chr->getType()->name(),
                                                event));
        } else if (taken == -2) {
            // FIXME This may not be an error if we allow IG entities
            // to belong to multiple accounts
            log(ERROR, String::compose("Connection(%1) requested to "
                                       "remove active character %2(%3) "
                                       "which is subscribed to another "
                                       "Connection(%4).", getId(),
                                       chr->getType()->name(),
                                       chr->getId(),
                                       chr->m_externalMind->connectionId()));
        }
    }
    return;
}

void Connection::setPossessionEnabled(bool enabled, const std::string& routerId)
{
    if (enabled) {
        ExternalMindsConnection connection(this, routerId);
        auto result = ExternalMindsManager::instance()->addConnection(connection);
        if (result == 0) {
            m_possessionRouters.push_back(routerId);
        }
    } else {
        auto result = std::find(m_possessionRouters.begin(), m_possessionRouters.end(), routerId);
        if (result != m_possessionRouters.end()) {
            ExternalMindsManager::instance()->removeConnection(routerId);
        }
    }
}


void Connection::addEntity(LocatedEntity * ent)
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


void Connection::externalOperation(const Operation & op, Link & link)
{
    debug(std::cout << "Connection::externalOperation"
                    << std::endl << std::flush;);
    if (op->isDefaultFrom()) {
        debug(std::cout << "deliver locally" << std::endl << std::flush;);
        OpVector reply;
        long serialno = op->getSerialno();
        operation(op, reply);
        OpVector::const_iterator Iend = reply.end();
        for(OpVector::const_iterator I = reply.begin(); I != Iend; ++I) {
            if (!op->isDefaultSerialno()) {
                // Should we respect existing refnos?
                if ((*I)->isDefaultRefno()) {
                    (*I)->setRefno(serialno);
                }
            }
            // FIXME detect socket failure here
            send(*I);
        }
        return;
    }
    const std::string & from = op->getFrom();
    debug(std::cout << "send on to " << from << std::endl << std::flush;);
    RouterMap::const_iterator I = m_objects.find(integerId(from));
    if (I == m_objects.end()) {
        sendError(op, String::compose("Client \"%1\" op from \"%2\" is from "
                                      "non-existant object.",
                                      op->getParents().front(), from), from);
        return;
    }
    I->second->externalOperation(op, link);
}

void Connection::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Connection::operation" << std::endl << std::flush;);
    auto op_no = op->getClassNo();
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
            std::string parent = op->getParents().empty() ? "-" : op->getParents().front();
            error(op, String::compose("Unknown operation %1 in Connection", parent), res);
            break;
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
        LocatedEntity * ent = J->second;
        Character * chr = dynamic_cast<Character *>(ent);
        // This code removes from this connection any of the accounts IG
        // entities, except one which is currently in use by this connection.
        // IE A player can log out their account, but continue using their
        // exisiting character. The only case where removeObject() is not
        // called is if the character has an external mind linked to this
        // connection.
        if (chr != 0) {
            if (chr->m_externalMind != 0) {
                if (!chr->m_externalMind->isLinked()) {
                    // FIXME This is probably not an error. This can happen
                    // if the account used this character in a previous
                    // session, but not in this one. The ExternalMind gets
                    // left behind, but would not be linked if it wasn't used
                    // this time.
                    log(ERROR, compose("Connection(%1) has found a "
                                       "character in its dictionery "
                                       "which is not connected.",
                                       getId()));
                    removeObject(ent->getIntId());
                } else if (!chr->m_externalMind->isLinkedTo(this)) {
                    // FIXME This is not an error _if_ we allow IG entities
                    // to be owned by multiple accounts.
                    log(ERROR, compose("Connection(%1) has found a "
                                       "character in its dictionery "
                                       "which is connected to another "
                                       "Connection(%2)", getId(),
                                       chr->m_externalMind->connectionId()));
                    removeObject(ent->getIntId());
                }
            } else {
                removeObject(ent->getIntId());
            }
        } else {
            // Non character entity
            removeObject(ent->getIntId());
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
