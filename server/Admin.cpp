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

static const bool debug_flag = true;

Admin::Admin(Connection * conn, const std::string& username,
             const std::string& passwd, const std::string & id) :
             Account(conn, username, passwd, id)
{
    subscribe("get", OP_GET);
    subscribe("set", OP_SET);
}

Admin::~Admin()
{
}

const char * Admin::getType() const
{
    return "admin";
}

OpVector Admin::characterError(const Create & op,
                               const Element::MapType & ent) const
{
    Element::MapType::const_iterator I = ent.find("parents");
    if ((I == ent.end()) || !I->second.IsList()) {
        return error(op, "You cannot create a character with no type.");
    }
    const Element::ListType & parents = I->second.AsList();
    if (parents.empty() || !parents.front().IsString()) {
        return error(op, "You cannot create a character with non-string type.");
    }
    return OpVector();
}

OpVector Admin::LogoutOperation(const Logout & op)
{
    const Element::ListType & args = op.GetArgs();
    
    if (args.empty() || !args.front().IsMap()) {
        return Account::LogoutOperation(op);
    } else {
        Element::MapType::const_iterator I = args.front().AsMap().find("id");
        if ((I == args.front().AsMap().end()) || (!I->second.IsString())) {
            return error(op, "No account id given");
        }
        if (m_connection == NULL) {
            return error(op, "Disconnected admin account handling explicit logout");
        }
        const std::string & account_id = I->second.AsString();
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
    const Element::ListType & args = op.GetArgs();
    if (args.empty()) {
        return error(op, "Get has no args.");
    }
    const Element & ent = args.front();
    if (!ent.IsMap()) {
        return error(op, "Get arg is not a map.");
    }
    const Element::MapType & emap = ent.AsMap();
    Element::MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.IsString()) {
        return error(op, "Get arg has no objtype.");
    }
    const std::string & objtype = I->second.AsString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.IsString()) {
        return error(op, "Get arg has no id.");
    }
    const std::string & id = I->second.AsString();
    if (id.empty()) {
        return error(op, "query id invalid");
    }
    Info * info = new Info(Info::Instantiate());
    if (objtype == "object") {
        const BaseDict & OOGDict = m_connection->m_server.getObjects();
        BaseDict::const_iterator J = OOGDict.find(id);
        const EntityDict & worldDict = m_connection->m_server.m_world.getObjects();
        EntityDict::const_iterator K = worldDict.find(id);

        Element::ListType & info_args = info->GetArgs();
        info_args.push_back(Element::MapType());
        if (J != OOGDict.end()) {
            J->second->addToObject(info_args.front().AsMap());
        } else if (K != worldDict.end()) {
            K->second->addToObject(info_args.front().AsMap());
        } else {
            delete info;
            return error(op, "Get id not found");
        }
    } else if ((objtype == "class") ||
               (objtype == "meta") ||
               (objtype == "op_definition")) {
        Atlas::Objects::Root * o = Inheritance::instance().get(id);
        if (o == NULL) {
            delete info;
            return error(op, "Unknown type definition requested");
        }
        Element::ListType & iargs = info->GetArgs();
        iargs.push_back(o->AsObject());
    } else {
        delete info;
        return error(op, "Unknow object type requested");
    }
    info->SetRefno(op.GetSerialno());
    info->SetSerialno(m_connection->m_server.getSerialNo());
    return OpVector(1,info);
}

OpVector Admin::SetOperation(const Set & op)
{
    const Element::ListType & args = op.GetArgs();
    if (args.empty()) {
        return error(op, "Set has no args.");
    }
    const Element & ent = args.front();
    if (!ent.IsMap()) {
        return error(op, "Set arg is not a map.");
    }
    const Element::MapType & emap = ent.AsMap();
    Element::MapType::const_iterator I = emap.find("objtype");
    if (I == emap.end() || !I->second.IsString()) {
        return error(op, "Set arg has no objtype.");
    }
    const std::string & objtype = I->second.AsString();
    I = emap.find("id");
    if (I == emap.end() || !I->second.IsString()) {
        return error(op, "Set arg has no id.");
    }
    const std::string & id = I->second.AsString();

    if (objtype == "object") {
        // Manipulate attributes of existing objects.
    } else if (objtype == "class") {
        // Install a new type from the client
        // It is possible this should actually be a create op that does
        // this. If so, set could perhaps be used to change things
        // in existing classes.
        // const std::string & parent = emap.find("parents")->second.AsList().front().AsString();
        // std::string script;
        // Element::MapType::const_iterator I = emap.find("script");
        // if ((I != emap.end()) && I->second.IsString()) {
        // script = I->second.AsString();
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
    const Element::ListType & args = op.GetArgs();
    if ((args.empty()) || (!args.front().IsMap())) {
        return OpVector();
    }

    const Element::MapType & entmap = args.front().AsMap();
    Element::MapType::const_iterator I = entmap.find("parents");
    if ((I == entmap.end()) || !(I->second.IsList()) ||
        (I->second.AsList().empty()) ||
        !(I->second.AsList().front().IsString()) ) {
        return error(op, "Character has no type");
    }

    return Account::CreateOperation(op);
}

// There used to be a code operation handler here. It may become desirable in
// the future for the admin account to be able to send script fragments.
// Think about implementing this.
