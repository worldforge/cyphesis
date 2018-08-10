#include <utility>

/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "AdminMind.h"

#include "LocatedEntity.h"

#include "common/debug.h"
#include "rulesets/BaseWorld.h"

#include "common/Link.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"
#include "common/custom.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

AdminMind::AdminMind(std::string strId, long id, LocatedEntity& entity)
    : ExternalMind(std::move(strId), id, entity)
{

}

void AdminMind::externalOperation(const Operation& op, Link& link)
{
    // If an admin connection specifies a TO on the op, we treat
    // it specially, and make sure it goes direct, otherwise
    // we handle it like a normal mind.
    debug_print("AdminMind::externalOperation(" << op->getParent() << ")");

    //TODO: The old Creator code had features for searching for entities.
    //TODO: Instead of retaining them we should add code in the Admin class
    //TODO: for searching for entities using a random EntityFilter expression
    if (op->isDefaultTo()) {
        debug_print("AdminMind handling op normally");
        ExternalMind::externalOperation(op, link);
    } else if (op->getTo() == getId() && op->isDefaultFutureSeconds()) {
        //Send directly to the entity, bypassing any normal Thought filtering
        debug_print("Creator handling op ");
        OpVector lres;
        m_entity.operation(op, lres);
    } else {
        //Otherwise we're sending something to someone else.
        auto to = BaseWorld::instance().getEntity(op->getTo());
        if (to) {
            //If it's a relay op we should send it as if it came from ourselves,
            // so the receiving entity knows where to return it.
            if (op->getClassNo() == Atlas::Objects::Operation::RELAY_NO) {
                //Make sure that "from_id" is set to this mind, so the Relay can find its way back.
                op->setAttr("from_id", getId());
                m_entity.sendWorld(op);
            } else {
                // Make it appear like it came from target itself;
                to->sendWorld(op);

//                //Send a sight of the operation to the mind
//                //NOTE: Is this really a good idea? There's no serial number set, so the op
//                //will be handled by the client as a valid op, even though it might not in reality
//                //always succeed (depending on game rules etc.).
//                Sight sight;
//                sight->setArgs1(op);
//                sight->setTo(getId());
//                OpVector res;
//                link.send(sight);
            }

        } else {
            log(ERROR, String::compose("Creator operation from client "
                                       "is to unknown ID \"%1\"",
                                       op->getTo()));

            Anonymous unseen_arg;
            unseen_arg->setId(op->getTo());

            Unseen unseen;
            unseen->setArgs1(unseen_arg);
            unseen->setTo(getId());
            if (!op->isDefaultSerialno()) {
                unseen->setRefno(op->getSerialno());
            }
            link.send(unseen);
        }
    }

}

void AdminMind::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ExternalMind::addToEntity(ent);
    ent->setAttr("admin", 1);
}

