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

#include "Admin.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "Ruleset.h"
#include "CommPeer.h"
#include "CommServer.h"
#include "Peer.h"

#include "rulesets/Entity.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/serialno.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"

#include "common/Connect.h"
#include "common/Monitor.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/functors/mem_fun.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using String::compose;

static const bool debug_flag = false;

/// \brief Admin constructor
Admin::Admin(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

Admin::~Admin()
{
    if (m_monitorConnection.connected()) {
        m_monitorConnection.disconnect();
    }
}

const char * Admin::getType() const
{
    return "admin";
}

static void addTypeToList(const Root & type, ListType & typeList)
{
    typeList.push_back(type->getId());
    Element children;
    if (type->copyAttr("children", children) != 0) {
        return;
    }
    if (!children.isList()) {
        log(ERROR, compose("Type %1 children attribute has type %2 instead of "
                           "string.", type->getId(),
                           Element::typeName(children.getType())));
        return;
    }
    ListType::const_iterator I = children.List().begin();
    ListType::const_iterator Iend = children.List().end();
    for (; I != Iend; ++I) {
        Root child = Inheritance::instance().getClass(I->asString());
        if (!child.isValid()) {
            log(ERROR, compose("Unable to find %1 in inheritance table",
                               I->asString()));
            continue;
        }
        addTypeToList(child, typeList);
    }
}

void Admin::addToMessage(MapType & omap) const
{
    Account::addToMessage(omap);
    ListType & typeList = (omap["character_types"] = ListType()).asList();
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
}

void Admin::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    Account::addToEntity(ent);
    ListType typeList;
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
    ent->setAttr("character_types", typeList);
}

/// \brief Function to monitor server operations
///
/// This function is connected to the WorldRouter operation dispatch
/// signal when monitoring is enabled, and relays all in-game operations
/// to the client.
void Admin::opDispatched(Operation op)
{
    if (m_connection != 0) {
        m_connection->send(op);
    } else {
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

int Admin::characterError(const Operation & op,
                          const RootEntity & ent, OpVector & res) const
{
    if (!ent->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
        error(op, "You cannot create a character with no type.", res, getId());
        return true;
    }
    const std::list<std::string> & parents = ent->getParents();
    if (parents.empty()) {
        error(op, "You cannot create a character with empty type.", res, getId());
        return true;
    }
    return false;
}

void Admin::LogoutOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    
    if (args.empty()) {
        Account::LogoutOperation(op, res);
        return;
    }

    if (m_connection == NULL) {
        error(op,"Disconnected admin account handling explicit logout",res, getId());
        return;
    }

    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "No account id given on logout op", res, getId());
        return;
    }
    const std::string & account_id = arg->getId();
    if (account_id == getId()) {
       Account::LogoutOperation(op, res);
    }
    Router * account = m_connection->m_server.getObject(account_id);
    if (!account) {
        error(op, "Logout failed", res, getId());
        return;
    }
    account->operation(op, res);
}

void Admin::GetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Get has no args.", res, getId());
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Get arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Get arg has no id.", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    if (id.empty()) {
        error(op, "Get arg id empty", res, getId());
        return;
    }
    Info info;
    if (objtype == "object" || objtype == "obj") {
        if (m_connection == 0) {
            return;
        }
        long intId = integerId(id);

        const RouterMap & OOGDict = m_connection->m_server.getObjects();
        RouterMap::const_iterator J = OOGDict.find(intId);
        const EntityDict & worldDict = m_connection->m_server.m_world.getEntities();
        EntityDict::const_iterator K = worldDict.find(intId);

        if (J != OOGDict.end()) {
            Router * obj = J->second;
            Anonymous info_arg;
            obj->addToEntity(info_arg);
            info->setArgs1(info_arg);
        } else if (K != worldDict.end()) {
            Anonymous info_arg;
            K->second->addToEntity(info_arg);
            info->setArgs1(info_arg);
        } else {
            clientError(op, compose("Unknown object id \"%1\" requested", id),
                        res, getId());
            return;
        }
    } else if (objtype == "class" ||
               objtype == "meta" ||
               objtype == "op_definition") {
        const Root & o = Inheritance::instance().getClass(id);
        if (!o.isValid()) {
            clientError(op, compose("Unknown type definition for \"%1\" "
                                    "requested", id), res);
            return;
        }
        info->setArgs1(o);
    } else {
        error(op, compose("Unknown object type \"%1\" requested for \"%2\"",
                          objtype, id), res, getId());
        return;
    }
    res.push_back(info);
}

void Admin::SetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "Set has no args.", res, getId());
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Set arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Set arg has no id.", res, getId());
        return;
    }
    const std::string & id = arg->getId();

    if (objtype == "object" || objtype == "obj") {

        long intId = integerId(id);

        if (m_charactersDict.find(intId) != m_charactersDict.end()) {
            Account::SetOperation(op, res);
            return;
        }
        log(WARNING, "Unable to set attributes of non-character yet");
        // Manipulate attributes of existing objects.
    } else if (objtype == "class" || objtype == "op_definition") {
        if (Inheritance::instance().hasClass(id)) {
            if (Ruleset::instance()->modifyRule(id, arg) == 0) {
                Info info;
                info->setTo(getId());
                info->setArgs1(arg);
                res.push_back(info);
            } else {
                error(op, "Updating type failed", res, getId());
            }
            return;
        }
        error(op, "Client attempting to use obsolete Set to install new type",
              res, getId());
        return;
    } else {
        error(op, "Unknow object type set", res, getId());
        return;
    }
}

void Admin::CreateOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }

    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) ||
        arg->getParents().empty()) {
        error(op, "Object to be created has no type", res, getId());
        return;
    }
    const std::string & parent = arg->getParents().front();

    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Object to be created has no objtype", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (objtype == "class" || objtype == "op_definition") {
        // New entity type
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Set arg has no id.", res, getId());
            return;
        }
        const std::string & id = arg->getId();

        if (parent.empty()) {
            error(op, "Attempt to install type with empty parent", res,
                  getId());
            return;
        }
        if (Inheritance::instance().hasClass(id)) {
            error(op, "Attempt to install type that already exists", res,
                  getId());
            return;
        }
        const Root & o = Inheritance::instance().getClass(parent);
        if (!o.isValid()) {
            error(op, compose("Attempt to install type with non-existant "
                              "parent \"%1\"", parent), res, getId());
            return;
        }
        if (Ruleset::instance()->installRule(id, arg) == 0) {
            Info info;
            info->setTo(getId());
            info->setArgs1(arg);
            res.push_back(info);
        } else {
            error(op, "Installing new type failed", res, getId());
        }
    } else {
        Account::CreateOperation(op, res);
    }
}

void Admin::OtherOperation(const Operation & op, OpVector & res)
{
    const int op_type = op->getClassNo();
    if (op_type == Atlas::Objects::Operation::CONNECT_NO) {
        return customConnectOperation(op, res);
    } else if (op_type == Atlas::Objects::Operation::MONITOR_NO) {
        return customMonitorOperation(op, res);
    }
}

/// \brief Process a Connect operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Admin::customConnectOperation(const Operation & op, OpVector & res)
{
    if (m_connection == 0) {
        log(ERROR, "Attempt to make peer connection from unconnected account");
        return;
    }

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "No argument to connect op", res, getId());
        return;
    }
    const Root & arg = args.front();
    Element hostname_attr;
    if (arg->copyAttr("hostname", hostname_attr) != 0 ||
        !hostname_attr.isString()) {
        error(op, "Argument to connect op has no hostname", res, getId());
        return;
    }
    const std::string & hostname = hostname_attr.String();

    Element port_attr;
    if (arg->copyAttr("port", port_attr) != 0 || !port_attr.isInt()) {
        error(op, "Argument to connect op has no port", res, getId());
        return;
    }
    int port = port_attr.Int();

    Element username_attr;
    if (arg->copyAttr("username", username_attr) != 0 || !username_attr.isString()) {
        error(op, "Argument to connect op has no username", res, getId());
        return;
    }
    const std::string & username = username_attr.String();

    Element password_attr;
    if (arg->copyAttr("password", password_attr) != 0 || !password_attr.isString()) {
        error(op, "Argument to connect op has no password", res, getId());
        return;
    }
    const std::string & password = password_attr.String();

    std::string peerId;
    long peer_iid = newId(peerId);
    if (peer_iid < 0) {
        error(op, "Connection failed as no ID available", res, getId());
        return;
    }

    CommPeer * cp = new CommPeer(m_connection->m_commClient.m_commServer,
                                 username, password);
    debug(std::cout << "Connecting to " << hostname << std::endl << std::flush;);
    if (cp->connect(hostname, port) != 0) {
        error(op, "Connection failed", res, getId());
        return;
    }
    log(INFO, String::compose("Connection succeeded %1", peerId));
    Peer * peer = new Peer(*cp, m_connection->m_server,
                           hostname, peerId, peer_iid);
    cp->setup(peer);
    m_connection->m_commClient.m_commServer.addSocket(cp);
    m_connection->m_commClient.m_commServer.addIdle(cp);
    m_connection->m_server.addObject(peer);
    // Fix it up
}

/// \brief Process a Monitor operation
///
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Admin::customMonitorOperation(const Operation & op, OpVector & res)
{
    if (!op->getArgs().empty()) {
        if (m_connection != 0) {
            if (!m_monitorConnection.connected()) {
                m_monitorConnection = m_connection->m_server.m_world.Dispatching.connect(sigc::mem_fun(this, &Admin::opDispatched));
            }
        }
    } else {
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admin account to be able to send script fragments.
// Think about implementing this.
