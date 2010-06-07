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

#include "Server.h"

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

/// \brief Server constructor
Server::Server(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

Server::~Server()
{
}

const char * Server::getType() const
{
    return "server";
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

void Server::addToMessage(MapType & omap) const
{
    Account::addToMessage(omap);
    ListType & typeList = (omap["character_types"] = ListType()).asList();
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
}

void Server::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    Account::addToEntity(ent);
    ListType typeList;
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
    ent->setAttr("character_types", typeList);
}

int Server::characterError(const Operation & op,
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

void Server::CreateOperation(const Operation & op, OpVector & res)
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
