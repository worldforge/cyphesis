// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Admin.h"

#include "ServerRouting.h"
#include "Connection.h"

#include "rulesets/Character.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/inheritance.h"
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Create.h>

#include <sigc++/object_slot.h>

static const bool debug_flag = true;

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
    std::cout << "Monitor" << std::endl << std::flush;
    if (m_connection != 0) {
        std::cout << "Sending monitor op to client" << std::endl << std::flush;
        m_connection->send(*op);
    } else {
        std::cout << "OtherOp.monitor auto unsubbing" << std::endl << std::flush;
        if (m_monitorConnection.connected()) {
            m_monitorConnection.disconnect();
        }
    }
}

OpVector Admin::characterError(const Create & op,
                               const Element::MapType & ent) const
{
    Element::MapType::const_iterator I = ent.find("parents");
    if ((I == ent.end()) || !I->second.isList()) {
        return error(op, "You cannot create a character with no type.");
    }
    const Element::ListType & parents = I->second.asList();
    if (parents.empty() || !parents.front().isString()) {
        return error(op, "You cannot create a character with non-string type.");
    }
    return OpVector();
}

OpVector Admin::LogoutOperation(const Logout & op)
{
    const Element::ListType & args = op.getArgs();
    
    if (args.empty() || !args.front().isMap()) {
        return Account::LogoutOperation(op);
    } else {
        Element::MapType::const_iterator I = args.front().asMap().find("id");
        if ((I == args.front().asMap().end()) || (!I->second.isString())) {
            return error(op, "No account id given");
        }
        if (m_connection == NULL) {
            return error(op, "Disconnected admin account handling explicit logout");
        }
        const std::string & account_id = I->second.asString();
        if (account_id == getId()) {
           return Account::LogoutOperation(op);
        }
        BaseEntity * player = m_connection->m_server.getObject(account_id);
        if (!player) {
            return error(op, "Logout failed");
        }
        return player->operation(op);
    }
}

OpVector Admin::GetOperation(const Get & op)
{
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
        return error(op, "Get has no args.");
    }
    const Element & ent = args.front();
    if (!ent.isMap()) {
        return error(op, "Get arg is not a map.");
    }
    const Element::MapType & emap = ent.asMap();
    Element::MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.isString()) {
        return error(op, "Get arg has no objtype.");
    }
    const std::string & objtype = I->second.asString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.isString()) {
        return error(op, "Get arg has no id.");
    }
    const std::string & id = I->second.asString();
    if (id.empty()) {
        return error(op, "query id invalid");
    }
    Info * info = new Info;
    if (objtype == "object") {
        const BaseDict & OOGDict = m_connection->m_server.getObjects();
        BaseDict::const_iterator J = OOGDict.find(id);
        const EntityDict & worldDict = m_connection->m_server.m_world.getObjects();
        EntityDict::const_iterator K = worldDict.find(id);

        Element::ListType & info_args = info->getArgs();
        info_args.push_back(Element::MapType());
        if (J != OOGDict.end()) {
            J->second->addToObject(info_args.front().asMap());
        } else if (K != worldDict.end()) {
            K->second->addToObject(info_args.front().asMap());
        } else {
            delete info;
            std::string msg("Unknown object id \"");
            msg += id;
            msg += "\" requested";
            return error(op, msg.c_str());
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
            return error(op, msg.c_str());
        }
        Element::ListType & iargs = info->getArgs();
        iargs.push_back(o->asObject());
    } else {
        delete info;
        std::string msg("Unknown object type \"");
        msg += objtype;
        msg += "\" requested for \"";
        msg += id;
        msg += "\"";
        return error(op, msg.c_str());
    }
    info->setRefno(op.getSerialno());
    info->setSerialno(m_connection->m_server.getSerialNo());
    return OpVector(1,info);
}

OpVector Admin::SetOperation(const Set & op)
{
    const Element::ListType & args = op.getArgs();
    if (args.empty()) {
        return error(op, "Set has no args.");
    }
    const Element & ent = args.front();
    if (!ent.isMap()) {
        return error(op, "Set arg is not a map.");
    }
    const Element::MapType & emap = ent.asMap();
    Element::MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.isString()) {
        return error(op, "Set arg has no objtype.");
    }
    const std::string & objtype = I->second.asString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.isString()) {
        return error(op, "Set arg has no id.");
    }
    // FIXME Use this id to install a type from the client
    // const std::string & id = I->second.asString();

    if (objtype == "object") {
        // Manipulate attributes of existing objects.
    } else if (objtype == "class") {
        // Install a new type from the client
        // It is possible this should actually be a create op that does
        // this. If so, set could perhaps be used to change things
        // in existing classes.
        // const std::string & parent = emap.find("parents")->second.asList().front().asString();
        // std::string script;
        // Element::MapType::const_iterator I = emap.find("script");
        // if ((I != emap.end()) && I->second.isString()) {
        // script = I->second.asString();
        // }
    } else if (objtype == "op_definition") {
        // Install a new op type? Perhaps again this should be a create.
    } else {
        return error(op, "Unknow object type set");
    }
    return OpVector();
}

OpVector Admin::CreateOperation(const Create & op)
{
    const Element::ListType & args = op.getArgs();
    if ((args.empty()) || (!args.front().isMap())) {
        return OpVector();
    }

    const Element::MapType & entmap = args.front().asMap();
    Element::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.isList()) ||
        (I->second.asList().empty()) ||
        !(I->second.asList().front().isString()) ) {
        return error(op, "Character has no type");
    }

    return Account::CreateOperation(op);
}

OpVector Admin::OtherOperation(const RootOperation & op)
{
    std::cout << "OtherOp" << std::endl << std::flush;
    const std::string & op_type = op.getParents().front().asString();
    if (op_type == "monitor") {
        std::cout << "OtherOp.monitor" << std::endl << std::flush;
        if (!op.getArgs().empty()) {
            std::cout << "OtherOp.monitor subbing" << std::endl << std::flush;
            if (m_connection != 0) {
                std::cout << "OtherOp.monitor cnnectd" << std::endl << std::flush;
                if (!m_monitorConnection.connected()) {
                    m_monitorConnection = m_connection->m_server.m_world.Dispatching.connect(SigC::slot(*this, &Admin::opDispatched));
                } else {
                    std::cout << "Already connected" << std::endl << std::flush;
                }
            }
        } else {
            std::cout << "OtherOp.monitor unsubbing" << std::endl << std::flush;
            if (m_monitorConnection.connected()) {
                m_monitorConnection.disconnect();
            } else {
                std::cout << "Already disconnected" << std::endl << std::flush;
            }
        }
    }
    return OpVector();
}
// There used to be a code operation handler here. It may become desirable in
// the future for the admin account to be able to send script fragments.
// Think about implementing this.
