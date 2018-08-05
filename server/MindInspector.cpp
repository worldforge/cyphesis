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
#endif

#include "MindInspector.h"

#include "rulesets/World.h"

#include "rulesets/BaseWorld.h"
#include "common/custom.h"
#include "common/Commune.h"
#include "common/Think.h"

#include <Atlas/Objects/Operation.h>

MindInspector::MindInspector() :
        m_serial(0)
{
}

MindInspector::~MindInspector()
{
}

void MindInspector::queryEntityForThoughts(const std::string& entityId)
{
    auto entity = BaseWorld::instance().getEntity(entityId);
    if (entity) {
        Atlas::Objects::Operation::Think think;
        Atlas::Objects::Operation::Get get;
        think->setArgs1(get);

        //Now find the World
        World* world = dynamic_cast<World*>(BaseWorld::instance().getEntity(0L).get());
        if (!world) {
            log(ERROR, "Could not get world; this is unusual.");
            return;
        }

        //The world is special in that it allows us to relay operations to in game entities.
        world->sendRelayToEntity(*entity, think,
                sigc::mem_fun(*this, &MindInspector::relayResponseReceived));

    }
}

void MindInspector::relayResponseReceived(const Operation& op,
        const std::string& entityId)
{
    ThoughtsReceived.emit(entityId, op);
}

