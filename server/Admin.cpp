// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Admin.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "EntityFactory.h"

#include "rulesets/Character.h"

#include "common/log.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation/Info.h>

#include <sigc++/object_slot.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Operation::Info;

static const bool debug_flag = false;

Admin::Admin(Connection * conn, const std::string& username,
             const std::string& passwd, const std::string & id) :
             Account(conn, username, passwd, id)
{
    subscribe("get", OP_GET);
    subscribe("set", OP_SET);
    subscribe("monitor", OP_MONITOR);
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

void Admin::opDispatched(RootOperation * op)
{
    if (m_connection != 0) {
        m_connection->send(*op);
    } else {
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

int Admin::characterError(const RootOperation & op,
                          const MapType & ent, OpVector & res) const
{
    MapType::const_iterator I = ent.find("parents");
    if ((I == ent.end()) || !I->second.isList()) {
        error(op, "You cannot create a character with no type.", res, getId());
        return true;
    }
    const ListType & parents = I->second.asList();
    if (parents.empty() || !parents.front().isString()) {
        error(op, "You cannot create a character with non-string type.", res, getId());
        return true;
    }
    return false;
}

void Admin::LogoutOperation(const RootOperation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    
    if (args.empty() || !args.front().isMap()) {
        Account::LogoutOperation(op, res);
    } else {
        MapType::const_iterator I = args.front().asMap().find("id");
        if ((I == args.front().asMap().end()) || (!I->second.isString())) {
            error(op, "No account id given", res, getId());
            return;
        }
        if (m_connection == NULL) {
            error(op,"Disconnected admin account handling explicit logout",res, getId());
            return;
        }
        const std::string & account_id = I->second.asString();
        if (account_id == getId()) {
           Account::LogoutOperation(op, res);
        }
        BaseEntity * player = m_connection->m_server.getObject(account_id);
        if (!player) {
            error(op, "Logout failed", res, getId());
            return;
        }
        player->operation(op, res);
    }
}

void Admin::GetOperation(const RootOperation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if (args.empty()) {
        error(op, "Get has no args.", res, getId());
        return;
    }
    const Element & ent = args.front();
    if (!ent.isMap()) {
        error(op, "Get arg is not a map.", res, getId());
        return;
    }
    const MapType & emap = ent.asMap();
    MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.isString()) {
        error(op, "Get arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = I->second.asString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.isString()) {
        error(op, "Get arg has no id.", res, getId());
        return;
    }
    const std::string & id = I->second.asString();
    if (id.empty()) {
        error(op, "query id invalid", res, getId());
        return;
    }
    Info * info = new Info;
    if ((objtype == "object") || (objtype == "obj")) {
        const BaseDict & OOGDict = m_connection->m_server.getObjects();
        BaseDict::const_iterator J = OOGDict.find(id);
        const EntityDict & worldDict = m_connection->m_server.m_world.getObjects();
        EntityDict::const_iterator K = worldDict.find(id);

        ListType & info_args = info->getArgs();
        info_args.push_back(MapType());
        if (J != OOGDict.end()) {
            J->second->addToMessage(info_args.front().asMap());
        } else if (K != worldDict.end()) {
            K->second->addToMessage(info_args.front().asMap());
        } else {
            delete info;
            std::string msg("Unknown object id \"");
            msg += id;
            msg += "\" requested";
            error(op, msg.c_str(), res, getId());
            return;
        }
    } else if ((objtype == "class") ||
               (objtype == "meta") ||
               (objtype == "op_definition")) {
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o == NULL) {
            delete info;
            std::string msg("Unknown type definition for \"");
            msg += id;
            msg += "\" requested";
            error(op, msg.c_str(), res, getId());
            return;
        }
        ListType & iargs = info->getArgs();
        iargs.push_back(o->asObject());
    } else {
        delete info;
        std::string msg("Unknown object type \"");
        msg += objtype;
        msg += "\" requested for \"";
        msg += id;
        msg += "\"";
        error(op, msg.c_str(), res, getId());
        return;
    }
    info->setRefno(op.getSerialno());
    info->setSerialno(m_connection->m_server.newSerialNo());
    res.push_back(info);
}

void Admin::SetOperation(const RootOperation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if (args.empty()) {
        error(op, "Set has no args.", res, getId());
        return;
    }
    const Element & ent = args.front();
    if (!ent.isMap()) {
        error(op, "Set arg is not a map.", res, getId());
        return;
    }
    const MapType & emap = ent.asMap();
    MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.isString()) {
        error(op, "Set arg has no objtype.", res, getId());
        return;
    }
    const std::string & objtype = I->second.asString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.isString()) {
        error(op, "Set arg has no id.", res, getId());
        return;
    }
    const std::string & id = I->second.asString();
    // FIXME Use this id to install a type from the client
    // const std::string & id = I->second.asString();

    if ((objtype == "object") || (objtype == "obj")) {
        if (m_charactersDict.find(id) != m_charactersDict.end()) {
            Account::SetOperation(op, res);
            return;
        }
        log(WARNING, "Unable to set attributes of non-character yet");
        // Manipulate attributes of existing objects.
    } else if (objtype == "class") {
        // Quick hack. This should eventually use EntityFactory, but that
        // code needs description in a strange format for now.
        I = emap.find("parents");
        if (I == emap.end()) {
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
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o != 0) {
            error(op, "Attempt to install type that already exists", res, getId());
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
        FactoryBase * f = EntityFactory::instance()->getNewFactory(parent);
        if (f == 0) {
            std::string msg("Attempt to find factory for parent \"");
            msg += parent;
            msg += "\" failed.";
            error(op, msg.c_str(), res, getId());
            return;
        }
        debug(std::cout << "Install type \"" << id << "\" with parent \""
                        << parent << "\"" << std::endl << std::flush;);
        EntityFactory::instance()->installFactory(parent, id, f);
    } else if (objtype == "op_definition") {
        // Install a new op type? Perhaps again this should be a create.
    } else {
        error(op, "Unknow object type set", res, getId());
        return;
    }
}

void Admin::CreateOperation(const RootOperation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return;
    }

    const MapType & entmap = args.front().asMap();
    MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.isList()) ||
        (I->second.asList().empty()) ||
        !(I->second.asList().front().isString()) ) {
        error(op, "Character has no type", res, getId());
        return;
    }

    Account::CreateOperation(op, res);
}

void Admin::OtherOperation(const RootOperation & op, OpVector & res)
{
    const std::string & op_type = op.getParents().front().asString();
    if (op_type == "monitor") {
        if (!op.getArgs().empty()) {
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
}
// There used to be a code operation handler here. It may become desirable in
// the future for the admin account to be able to send script fragments.
// Think about implementing this.
