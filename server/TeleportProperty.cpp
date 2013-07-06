// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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


#include "server/TeleportProperty.h"

#include "server/Juncture.h"
#include "server/ServerRouting.h"

#include "rulesets/LocatedEntity.h"

#include "common/BaseWorld.h"
#include "common/compose.hpp"
#include "common/debug.h"
#include "common/log.h"
#include "common/Teleport.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

static const bool debug_flag = false;

void TeleportProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::TELEPORT_NO, name);
}

HandlerResult TeleportProperty::operation(LocatedEntity * ent,
                                          const Operation & op,
                                          OpVector & res)
{
    return TeleportProperty::teleport_handler(ent, op, res);
}

HandlerResult TeleportProperty::teleport_handler(LocatedEntity * e,
                                                 const Operation & op,
                                                 OpVector & res)
{
    ServerRouting *svr = ServerRouting::instance();
    if(svr == NULL) {
        log(ERROR, "Unable to access ServerRouting object");
        return OPERATION_IGNORED;
    }
    Router * obj = svr->getObject(data());
    if(obj == NULL) {
        log(ERROR, "Unknown peer ID specified");
        return OPERATION_IGNORED;
    }
    Juncture * link = dynamic_cast<Juncture *>(obj);
    if(link == NULL) {
        log(ERROR, "Non Peer ID specified");
        return OPERATION_IGNORED;
    }

    // Get the ID of the sender
    if (op->isDefaultFrom()) {
        debug(std::cout << "ERROR: Operation with no entity to be teleported" 
                        << std::endl << std::flush;);
        return OPERATION_IGNORED;
    }
    log(INFO, String::compose("Teleport request sender has ID %1",
                              op->getFrom()));

    // This is the sender entity
    LocatedEntity * entity = BaseWorld::instance().getEntity(op->getFrom());
    if (entity == 0) {
        debug(std::cout << "No entity found with the specified ID: "
                        << op->getFrom(););
        return OPERATION_IGNORED;
    }

    // Inject the entity into remote server
    link->teleportEntity(entity);
    return OPERATION_IGNORED;
}
