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


#include "ServerAccount.h"

#include "ServerRouting.h"
#include "Connection.h"
#include "PossessionAuthenticator.h"

#include "rulesets/LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/debug.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
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

int ServerAccount::characterError(const Operation & op,
                                  const Root & ent,
                                  OpVector & res) const
{
    return -1;
}

void ServerAccount::createObject(const std::string & type_str,
                                 const Root & arg,
                                 const Operation & op,
                                 OpVector & res)
{
// Format of the Create ops that are received by this function should
// have the entity to be created as the first argument. If the entity
// being created is a character associated with an account, an additional
// argument should specify the possess key that will be used by the client
// to claim ownership of the entity being created.

    if (arg->getObjtype() != "obj") {
        // Return error to peer
        error(op, "Only creation of entities by peer server is permitted",
              res, getId());
        return;
    }

    RootEntity ent = smart_dynamic_cast<RootEntity>(arg);
    if(!ent.isValid()) {
        log(ERROR, "Character creation arg is malformed");
        return;
    }

    // If we have a possess key (entity has a mind)
    PossessionAuthenticator * tele_auth = 0;
    std::string possess_key;

    const std::vector<Root> & args = op->getArgs();
    if (args.size() == 2) {
        const Root & arg2 = args.back();
        Element key;
        if(arg2->copyAttr("possess_key", key) == 0 && key.isString()) {
            possess_key = key.String();
            tele_auth = PossessionAuthenticator::instance();
        } else {
            log(ERROR, "Entity has mind but no possess key found");
            return;
        }
    }

    debug( std::cout << "ServerAccount creating a " << type_str << " object"
                     << std::endl << std::flush; );

    LocatedEntity * entity = addNewEntity(type_str, ent, ent);

    if (entity == 0) {
        error(op, "Character creation failed", res, getId());
        return;
    }

    if (tele_auth != 0) {
        if (tele_auth->addPossession(entity->getId(), possess_key) != 0) {
            // Delete the created entity on failure
            log(CRITICAL, "Unable to insert into PossessionAuthenticator");
            return;
        }
    }

    logEvent(IMPORT_ENT, String::compose("%1 %2 %3 Imported entity %4",
                                         m_connection->getId(),
                                         getId(),
                                         entity->getId(),
                                         arg->getId()));

    // The reply to a successful Create operation in the ServerAccount class
    // is an Info operation. The argument is the new 
    // entity that has been created. The refno is set to the serialno
    // of the incoming operation, which is the ID of the
    // entity on the sender server (used by the recipient of this reply to 
    // figure out which Create op this reply is associated with

    Anonymous info_arg;
    entity->addToEntity(info_arg);

    Info info;
    info->setArgs1(info_arg);
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }
    // TO/FROM?
    res.push_back(info);
}

/// \brief Add an entity to the world but don't add it to any particular account
///
/// \param typestr The type string of the entity
/// \param ent A container for the entity to be created in the world
/// \param arg The argument of the Create op containing the entity itself
LocatedEntity * ServerAccount::addNewEntity(const std::string & typestr,
                                            const RootEntity & ent,
                                            const Root & arg)
{
    if (!m_connection) {
        return nullptr;
    }
    BaseWorld & world = m_connection->m_server.m_world;
    debug(std::cout << "Account::Add_character" << std::endl << std::flush;);
    LocatedEntity * chr;
    Element spawn;
    if (arg->copyAttr("spawn_name", spawn) == 0 && spawn.isString()) {
        chr = world.spawnNewEntity(spawn.String(), typestr, ent);
    } else {
        chr = world.addNewEntity(typestr, ent);
    }
    if (!chr) {
        return nullptr;
    }
    debug(std::cout << "Added" << std::endl << std::flush;);
    assert(chr->m_location.isValid());
    debug(std::cout << "Location set to: " << chr->m_location << std::endl << std::flush;);
    return chr;
}

