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
#include "common/BaseWorld.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

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
        new_loc.m_velocity = Vector3D(0,0,0);
        Character* character = dynamic_cast<Character*>(e);
        if (character && (!character->m_externalMind || !character->m_externalMind->isLinked())) {
            //if it's a character we should check if it's externally controlled. If it's not
            //we should put the character in limbo until an external mind is connected.
            new_loc.m_loc = nullptr;
            if (!m_entityLinkConnection) {
                m_entityLinkConnection = character->externalLinkChanged.connect(*this, &RespawningProperty);
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

        //We need to move the entity to the spawn point instead of deleting it.
        //This will effectively make the system ignore the Delete op.
        return OPERATION_BLOCKED;
    }
    return OPERATION_IGNORED;
}



