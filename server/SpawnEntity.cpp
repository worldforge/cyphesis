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

// $Id$

#include "SpawnEntity.h"

#include "rulesets/Entity.h"
#include "rulesets/AreaProperty.h"

#include "common/log.h"
#include "common/random.h"
#include "common/type_utils_impl.h"

#include "physics/Shape.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <wfmath/polygon.h>

using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Create;

SpawnEntity::SpawnEntity(LocatedEntity * e) : m_ent(e)
{
}

int SpawnEntity::setup(const MapType & data)
{
    MapType::const_iterator I = data.find("character_types");
    MapType::const_iterator Iend = data.end();
    if (I != Iend && I->second.isList()) {
        m_characterTypes = I->second.List();
    }
    I = data.find("contains");
    if (I != Iend && I->second.isList()) {
        m_inventory = I->second.List();
    }
    return 0;
}

static const int check_character_type(const std::string & type,
                                      const Atlas::Message::ListType & types)
{
    ListType::const_iterator I = types.begin();
    ListType::const_iterator Iend = types.end();
    for (; I != Iend; ++I) {
        if (*I == type) {
            return 0;
        }
    }
    return -1;
}
                                  

int SpawnEntity::spawnEntity(const std::string & type,
                             const RootEntity & dsc)
{
    if (m_ent.get() == 0) {
        return -1;
    }
    if (check_character_type(type, m_characterTypes) != 0) {
        return -1;
    }
    dsc->setLoc(m_ent->m_location.m_loc->getId());
    const AreaProperty * ap = m_ent->getPropertyClass<AreaProperty>("area");
    if (ap != 0) {
        // FIXME orientation ignored
        const Area * spawn_area = ap->shape();
        WFMath::AxisBox<2> spawn_box = spawn_area->footprint();
        Point3D new_pos = m_ent->m_location.pos();
        for (int i = 0; i < 10; ++i) {
            WFMath::CoordType x = uniform(spawn_box.lowCorner().x(),
                                          spawn_box.highCorner().x());
            WFMath::CoordType y = uniform(spawn_box.lowCorner().y(),
                                          spawn_box.highCorner().y());
            if (spawn_area->intersect(WFMath::Point<2>(x, y))) {
                new_pos += Vector3D(x, y, 0);
                break;
            }
        }
        ::addToEntity(new_pos, dsc->modifyPos());
    } else if (m_ent->m_location.bBox().isValid()) {
        const BBox & b = m_ent->m_location.bBox();
        ::addToEntity(Point3D(uniform(b.lowCorner().x(), b.highCorner().x()),
                              uniform(b.lowCorner().y(), b.highCorner().y()),
                              0), dsc->modifyPos());
            // Locate in bbox
    } else {
        ::addToEntity(m_ent->m_location.pos(), dsc->modifyPos());
    }
    // FIXME this is exactly the same location as the spawn entity
    return 0;
}

int SpawnEntity::populateEntity(LocatedEntity * ent,
                                const RootEntity & dsc,
                                OpVector & res)
{
    // Hack in default objects
    // This needs to be done in a generic way
    Anonymous create_arg;
    ::addToEntity(Point3D(0,0,0), create_arg->modifyPos());
    create_arg->setLoc(ent->getId());

    ListType::const_iterator I = m_inventory.begin();
    ListType::const_iterator Iend = m_inventory.end();
    for (; I != Iend; ++I) {
        if (!I->isString()) {
            continue;
        }
        Create c;
        c->setTo(ent->getId());
        create_arg = create_arg.copy();
        create_arg->setParents(std::list<std::string>(1, I->String()));
        c->setArgs1(create_arg);
        res.push_back(c);
    }

    return 0;
}

int SpawnEntity::addToMessage(MapType & msg) const
{
    msg.insert(std::make_pair("character_types", m_characterTypes));
    return 0;
}
