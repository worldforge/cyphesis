// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Admin.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "EntityFactory.h"
#include "CommPeer.h"
#include "CommServer.h"

#include "rulesets/Entity.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/serialno.h"
#include "common/inheritance.h"
#include "common/compose.hpp"

#include "common/Connect.h"
#include "common/Monitor.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

Admin::Admin(Connection * conn, const std::string& username,
             const std::string& passwd, const std::string & id) :
             Account(conn, username, passwd, id)
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
    BaseEntity * account = m_connection->m_server.getObject(account_id);
    if (!account) {
        error(op, "Logout failed", res, getId());
        return;
    }
    account->operation(op, res);
}

void Admin::GetOperation(const Operation & op, OpVector & res)
{
    assert(m_connection != 0);
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
        error(op, "query id invalid", res, getId());
        return;
    }
    Info info;
    if ((objtype == "object") || (objtype == "obj")) {
        const BaseDict & OOGDict = m_connection->m_server.getObjects();
        BaseDict::const_iterator J = OOGDict.find(id);
        const EntityDict & worldDict = m_connection->m_server.m_world.getEntities();

        long intId = strtol(id.c_str(), 0, 10);
        if (intId == 0 && id != "0") {
            log(ERROR, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
        }

        EntityDict::const_iterator K = worldDict.find(intId);

        if (J != OOGDict.end()) {
            Anonymous info_arg;
            J->second->addToEntity(info_arg);
            info->setArgs1(info_arg);
        } else if (K != worldDict.end()) {
            Anonymous info_arg;
            K->second->addToEntity(info_arg);
            info->setArgs1(info_arg);
        } else {
            std::string msg("Unknown object id \"");
            msg += id;
            msg += "\" requested";
            error(op, msg.c_str(), res, getId());
            return;
        }
    } else if ((objtype == "class") ||
               (objtype == "meta") ||
               (objtype == "op_definition")) {
        const Root & o = Inheritance::instance().getClass(id);
        if (!o.isValid()) {
            std::string msg("Unknown type definition for \"");
            msg += id;
            msg += "\" requested";
            error(op, msg.c_str(), res, getId());
            return;
        }
        info->setArgs1(o);
    } else {
        std::string msg("Unknown object type \"");
        msg += objtype;
        msg += "\" requested for \"";
        msg += id;
        msg += "\"";
        error(op, msg.c_str(), res, getId());
        return;
    }
    info->setSerialno(newSerialNo());
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
    // FIXME Use this id to install a type from the client

    if ((objtype == "object") || (objtype == "obj")) {

        long intId = strtol(id.c_str(), 0, 10);
        if (intId == 0 && id != "0") {
            log(ERROR, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
        }

        if (m_charactersDict.find(intId) != m_charactersDict.end()) {
            Account::SetOperation(op, res);
            return;
        }
        log(WARNING, "Unable to set attributes of non-character yet");
        // Manipulate attributes of existing objects.
    } else if (objtype == "class") {
        if (Inheritance::instance().hasClass(id)) {
            if (EntityFactory::instance()->modifyRule(id, arg->asMessage()) == 0) {
                Info info;
                info->setTo(getId());
                info->setArgs1(arg);
                res.push_back(info);
            } else {
                error(op, "Unknown error updating type", res, getId());
            }
            return;
        }
        error(op, "Client using Set to install new type", res, getId());
        return;
#if 0
        // We no longer accept types installed using Set ops
        I = arg.find("parents");
        if (I == Iend) {
            error(op, "Attempt to install type with no parents", res, getId());
            return;
        }
        if (!I->second.isList()) {
            error(op, "Attempt to install type with non-list parents", res, getId());
            return;
        }
        const ListType & parents = I->second.asList();
        if (parents.empty() || !parents.front().isString()) {
            error(op, "Attempt to install type with invalid parent", res, getId());
            return;
        }
        const std::string & parent = parents.front().asString();
        if (parent.empty()) {
            error(op, "Attempt to install type with parent=\"\"", res, getId());
            return;
        }
        o = Inheritance::instance().get(parent);
        if (o == 0) {
            std::string msg("Attempt to install type with non-existant parent \"");
            msg += parent;
            msg += "\"";
            error(op, msg.c_str(), res, getId());
            return;
        }
        if (EntityFactory::instance()->installRule(id, arg) == 0) {
            Info info;
            info->setTo(getId());
            ListType & info_args = info->getArgs();
            info_args.push_back(arg);
            res.push_back(info);
        } else {
            error(op, "Unknown error installing new type", res, getId());
        }
#endif
    } else if (objtype == "op_definition") {
        // Install a new op type? Perhaps again this should be a create.
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
    if (objtype == "class") {
        // New entity type
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Set arg has no id.", res, getId());
            return;
        }
        const std::string & id = arg->getId();

        if (parent.empty()) {
            error(op, "Attempt to install type with empty parent", res, getId());
            return;
        }
        if (Inheritance::instance().hasClass(id)) {
            error(op, "Attempt to install type that already exists", res, getId());
            return;
        }
        const Root & o = Inheritance::instance().getClass(parent);
        if (!o.isValid()) {
            std::string msg("Attempt to install type with non-existant parent \"");
            msg += parent;
            msg += "\"";
            error(op, msg.c_str(), res, getId());
            return;
        }
        if (EntityFactory::instance()->installRule(id, arg->asMessage()) == 0) {
            Info info;
            info->setTo(getId());
            info->setArgs1(arg);
            res.push_back(info);
        } else {
            error(op, "Unknown error installing new type", res, getId());
        }
    } else if (objtype == "op_definition") {
        // New operation type
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

void Admin::customConnectOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "No argument to connect op", res, getId());
        return;
    }
    const Root & arg = args.front();
    Element hostname_attr;
    if (arg->copyAttr("hostname", hostname_attr) != 0) {
        error(op, "Argument to connect op has no hostname", res, getId());
        return;
    }
    if (!hostname_attr.isString()) {
        error(op, "Argument to connect op has non string hostname", res, getId());
        return;
    }
    if (m_connection == 0) {
        log(ERROR, "Attempt to make peer connection from unconnected account");
        return;
    }
    const std::string & hostname = hostname_attr.String();
    CommPeer * cp = new CommPeer(m_connection->m_commClient.m_commServer,
                                 hostname);
    std::cout << "Connecting to " << hostname << std::endl << std::flush;
    if (cp->connect(hostname) != 0) {
        error(op, "Connection failed", res, getId());
        return;
    }
    log(INFO, "Connection succeeded");
    cp->setup();
    m_connection->m_commClient.m_commServer.addSocket(cp);
    // Fix it up
}

void Admin::customMonitorOperation(const Operation & op, OpVector & res)
{
    if (!op->getArgs().empty()) {
        if (m_connection != 0) {
            if (!m_monitorConnection.connected()) {
                m_monitorConnection = m_connection->m_server.m_world.Dispatching.connect(SigC::slot(*this, &Admin::opDispatched));
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
