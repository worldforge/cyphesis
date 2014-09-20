/*
 Copyright (C) 2013 Erik Ogenvik

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RespawningProperty.h"
#include "LocatedEntity.h"
#include "Character.h"
#include "ExternalMind.h"
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <sigc++/bind.h>

static const bool debug_flag = false;

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Move;

RespawningProperty::RespawningProperty()
{
}

RespawningProperty::~RespawningProperty()
{
    m_entityLinkConnection.disconnect();
}

void RespawningProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::DELETE_NO, name);

    //Check if we're already in limbo.
    Character* character = dynamic_cast<Character*>(owner);
    if (character && BaseWorld::instance().getLimboLocation() && character->m_location.m_loc == BaseWorld::instance().getLimboLocation()) {
        if (!m_entityLinkConnection) {
            m_entityLinkConnection = character->externalLinkChanged.connect(sigc::bind(sigc::mem_fun(*this, &RespawningProperty::entity_externalLinkChanged), owner));
        }
    }
}

void RespawningProperty::remove(LocatedEntity *owner, const std::string & name)
{
    owner->removeDelegate(Atlas::Objects::Operation::DELETE_NO, name);
}

void RespawningProperty::apply(LocatedEntity * ent)
{

}

HandlerResult RespawningProperty::operation(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    return delete_handler(e, op, res);
}

RespawningProperty * RespawningProperty::copy() const
{
    return new RespawningProperty(*this);
}

HandlerResult RespawningProperty::delete_handler(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    if (!m_data.empty()) {

        Location new_loc;
        new_loc.m_velocity = Vector3D::ZERO();
        Character* character = dynamic_cast<Character*>(e);
        //if it's a character we should check if it's externally controlled. If it's not
        //we should put the character in limbo until an external mind is connected.
        if (character && (!character->m_externalMind || !character->m_externalMind->isLinked()) &&
                BaseWorld::instance().getLimboLocation()) {
            new_loc.m_loc = BaseWorld::instance().getLimboLocation();
            new_loc.m_pos = Point3D::ZERO();
            if (!m_entityLinkConnection) {
                m_entityLinkConnection = character->externalLinkChanged.connect(sigc::bind(sigc::mem_fun(*this, &RespawningProperty::entity_externalLinkChanged), e));
            }
        } else {
            if (BaseWorld::instance().moveToSpawn(m_data, new_loc) == 0) {
            } else {
                //TODO: handle that the entity now is in limbo
                new_loc.m_loc = nullptr;
            }
        }

        Anonymous move_arg;
        move_arg->setId(e->getId());
        new_loc.addToEntity(move_arg);
        Move move;
        move->setFrom(e->getId());
        move->setTo(e->getId());
        move->setArgs1(move_arg);
        res.push_back(move);

        //We need to move the entity to the spawn point, or to limbo, instead of deleting it.
        //This will effectively make the system ignore the Delete op.
        return OPERATION_BLOCKED;
    }
    return OPERATION_IGNORED;
}

void RespawningProperty::entity_externalLinkChanged(LocatedEntity* entity) {
    //When the entity gets externally controlled again we should move it out of limbo and back to the respawn.

    m_entityLinkConnection.disconnect();
    Location new_loc;
    new_loc.m_velocity = Vector3D::ZERO();

    BaseWorld::instance().moveToSpawn(m_data, new_loc);
    Anonymous move_arg;
    move_arg->setId(entity->getId());
    new_loc.addToEntity(move_arg);
    Move move;
    move->setFrom(entity->getId());
    move->setTo(entity->getId());
    move->setArgs1(move_arg);
    OpVector res;
    //Note that we're bypassing the normal operations flow. This is because we want this movement to happen
    //instantly, as the character is currently being attached to an external mind. We don't want the external mind
    //to first get information about the entity being contained in limbo, and then just thereafter being moved to
    //a spawn, since that would just make it harder for the client. Instead we want it to appear to the client
    //as if the entity always had been at the spawn point.
    entity->operation(move, res);
    for (auto& op : res) {
        BaseWorld::instance().message(op, *entity);
    }
}


