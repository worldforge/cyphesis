// Cyphesis Online RPG ServerAccount and AI Engine
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

#include "ServerAccount.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "Ruleset.h"
#include "CommPeer.h"
#include "CommServer.h"
#include "Peer.h"
#include "TeleportAuthenticator.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

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
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

using String::compose;

static const bool debug_flag = false;

/// \brief ServerAccount constructor
ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

ServerAccount::~ServerAccount()
{
}

/// \brief Get the type of account ("server" in this case)
///
/// \return The type string
const char * ServerAccount::getType() const
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

void ServerAccount::addToMessage(MapType & omap) const
{
    Account::addToMessage(omap);
    ListType & typeList = (omap["character_types"] = ListType()).asList();
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
}

void ServerAccount::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    Account::addToEntity(ent);
    ListType typeList;
    Root character_type = Inheritance::instance().getClass("character");
    if (character_type.isValid()) {
        addTypeToList(character_type, typeList);
    }
    ent->setAttr("character_types", typeList);
}

int ServerAccount::characterError(const Operation & op,
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

/// \brief This operation allows the creation of entities on the server
///
/// \param op The Create operation
/// \param res The result set of replies
void ServerAccount::CreateOperation(const Operation & op, OpVector & res)
{
    // Format of the Create ops that are received by this function should
    // have the entity to be created as the first argument. If the entity
    // being created is a character associated with an account, an additional
    // argument should specify the possess key that will be used by the client
    // to claim ownership of the entity being created.

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }
    RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if(!arg.isValid()) {
        log(ERROR, "Character creation arg is malformed");
        return;
    }

    const std::string & old_id = arg->getId();
    log(INFO, compose("Old entity had ID %1", old_id));

    if (!arg->hasAttrFlag(Atlas::Objects::OBJTYPE_FLAG)) {
        error(op, "Object to be created has no objtype", res, getId());
        return;
    }
    const std::string & objtype = arg->getObjtype();
    if (objtype != "obj") {
        log(INFO, "Only creation of entities currently supported");
        return;
    }

    if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
        error(op, "Entity has no type", res, getId());
        return;
    }

    const std::list<std::string> & parents = arg->getParents();
    if (parents.empty()) {
        error(op, "Entity has empty type list.", res, getId());
        return;
    }
    
    const std::string & typestr = parents.front();

    if (characterError(op, arg, res)) {
        return;
    }

    // If we have a possess key (entity has a mind)
    bool isMind = false;
    std::string possess_key;
    if (args.size() == 2) {
        RootEntity arg2 = smart_dynamic_cast<RootEntity>(args.back());
        Element key;
        if(arg2->copyAttr("possess_key", key) == 0 && key.isString()) {
            log(ERROR, "Entity has mind but no possess key found");
            return;
        }
        possess_key = key.String();
        isMind = true;
    }

    TeleportAuthenticator *tele_auth = NULL;
    if (isMind) {
        tele_auth = TeleportAuthenticator::instance();
        if (tele_auth == NULL) {
            log(ERROR, "Unable to retreive TeleportAuthenticator instance");
            return;
        }
    }

    debug( std::cout << "Account creating a " << typestr << " object"
                     << std::endl << std::flush; );

    Anonymous new_entity;
    new_entity->setParents(std::list<std::string>(1, typestr));
    new_entity->setAttr("status", 0.024);
    new_entity->setAttr("mind", "");
    if (!arg->isDefaultName()) {
        new_entity->setName(arg->getName());
    }

    Entity * entity = addNewEntity(typestr, new_entity, arg);

    if (entity == 0) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    if (isMind) {
        if (tele_auth->addTeleport(entity->getId(), possess_key) == -1) {
            // Delete the created entity on failure
            Delete delOp;
            Anonymous del_arg;
            del_arg->setId(id);
            delOp->setArgs1(del_arg);
            delOp->setTo(id);
            entity->sendWorld(delOp);
            log(ERROR, "Unable to insert into TeleportAuthenticator. Entity deleted.");
            return;
        }
    }

    // The reply to a successful Create operation in the ServerAccount class
    // is an Info operation with two arguments. The first argument is the new 
    // entity that has been created. The second argument is the ID of the
    // entity on the sender server (used by the recipient of this reply to 
    // figure out which Create op this reply is associated with
    Info info;
    std::vector<Root> reply_args;
    Anonymous info_arg;
    RootEntity prev_id;
    prev_id->setId(old_id);
    entity->addToEntity(info_arg);
    reply_args.push_back(info_arg);
    reply_args.push_back(prev_id);
    info->setArgs(reply_args);
    res.push_back(info);
}

/// \brief Add an entity to the world but don't add it to any particular account
///
/// \param typestr The type string of the entity
/// \param ent A container for the entity to be created in the world
/// \param arg The argument of the Create op containing the entity itself
Entity * ServerAccount::addNewEntity(const std::string & typestr,
                                  const RootEntity & ent,
                                  const RootEntity & arg)
{
    if (m_connection == 0) {
        return 0;
    }
    BaseWorld & world = m_connection->m_server.m_world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    Entity * chr;
    Element spawn;
    if (arg->copyAttr("spawn_name", spawn) == 0 && spawn.isString()) {
        chr = world.spawnNewEntity(spawn.String(), typestr, ent);
    } else {
        chr = world.addNewEntity(typestr, ent);
    }
    if (chr == 0) {
        return 0;
    }
    debug(std::cout << "Added" << std::endl << std::flush;);
    assert(chr->m_location.isValid());
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    return chr;
}

