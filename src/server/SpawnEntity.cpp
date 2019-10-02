// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "SpawnEntity.h"

#include "rules/LocatedEntity.h"
#include "rules/simulation/AreaProperty.h"

#include "common/random.h"

#include "physics/Shape.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/polygon.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;

SpawnEntity::SpawnEntity(LocatedEntity * e) :
        m_ent(e)
{
}

int SpawnEntity::setup(const MapType & data)
{
    auto I = data.find("entities");
    if (I != data.end() && I->second.isMap()) {
        const auto& entities = I->second.asMap();
        for (auto J : entities) {
            if (!J.second.isMap()) {
                log(ERROR,
                        "Entry in entities attribute of spawn not in map format.");
                continue;
            }

            m_entities.insert(std::make_pair(J.first, J.second.asMap()));
        }
    }
    return 0;
}

int SpawnEntity::spawnEntity(const std::string & type, const RootEntity & dsc) const
{
    if (!m_ent) {
        return -1;
    }

    auto entityI = m_entities.find(type);
    if (entityI == m_entities.end()) {
        return -1;
    }

    MapType entityTemplate = entityI->second;
    auto objtypeI = entityTemplate.find("objtype");
    if (objtypeI != entityTemplate.end()
            && objtypeI->second.asString() == "archetype") {
        if (dsc->getObjtype() != "archetype") {
            //if the template is an archetype, but the supplied entity isn't,
            //we need to move the default attributes from the root entity to
            //the first entity in the archetype
            auto entitiesI = entityTemplate.find("entities");
            if (entitiesI != entityTemplate.end() && entitiesI->second.isList()
                    && !entitiesI->second.asList().empty()) {
                auto& firstEntityElement = entitiesI->second.asList().front();
                MapType& firstEntity = firstEntityElement.asMap();

                MapType dscAttributes;
                dsc->addToMessage(dscAttributes);
                for (auto attributeEntry : dscAttributes) {
                    //only move those entities that aren't defined in the archetype
                    //(i.e. the archetype overrides what's sent)
                    if (firstEntity.find(attributeEntry.first)
                            == firstEntity.end()) {
                        firstEntity.insert(attributeEntry);
                        //once moved remove the attribute from the supplied entity
                        dsc->removeAttr(attributeEntry.first);
                    }
                }
            }
        }
    }


    for (const auto& entry : entityTemplate) {
        dsc->setAttr(entry.first, entry.second);
    }

    Location new_loc;
    placeInSpawn(new_loc);
    new_loc.addToEntity(dsc);

    return 0;
}

int SpawnEntity::addToMessage(MapType & msg) const
{
    if (!m_entities.empty()) {
        Atlas::Message::ListType keys(m_entities.size());
        for (const auto& entry : m_entities) {
            keys.push_back(entry.first);
        }
        msg.insert(std::make_pair("character_types", keys));
    }
    return 0;
}

int SpawnEntity::placeInSpawn(Location& location) const
{

    location.m_parent = m_ent->m_location.m_parent;
    const auto* ap = m_ent->getPropertyClassFixed<AreaProperty>();
    if (ap && ap->shape()) {
        // FIXME orientation ignored
        const Area * spawn_area = ap->shape();
        WFMath::AxisBox<2> spawn_box = spawn_area->footprint();
        Point3D new_pos = m_ent->m_location.pos();
        for (int i = 0; i < 10; ++i) {
            WFMath::CoordType x = uniform(spawn_box.lowCorner().x(),
                    spawn_box.highCorner().x());
            WFMath::CoordType z = uniform(spawn_box.lowCorner().y(),
                    spawn_box.highCorner().y());
            if (spawn_area->intersect(WFMath::Point<2>(x, z))) {
                new_pos += Vector3D(x, 0, z);
                break;
            }
        }
        location.m_pos = new_pos;
    } else if (m_ent->m_location.bBox().isValid()) {
        const BBox & b = m_ent->m_location.bBox();
        location.m_pos = Point3D(uniform(b.lowCorner().x(), b.highCorner().x()),
             0, uniform(b.lowCorner().z(), b.highCorner().z()));
        // Locate in bbox
    } else {
        location.m_pos = m_ent->m_location.pos();
    }
    return 0;
}

