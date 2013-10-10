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

#include "SpawnerProperty.h"
#include "LocatedEntity.h"

#include "common/Tick.h"
#include "common/TypeNode.h"
#include "common/const.h"
#include "common/BaseWorld.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <wfmath/MersenneTwister.h>
#include <wfmath/quaternion.h>
#include <wfmath/const.h>
#include <wfmath/atlasconv.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Factories;
using Atlas::Objects::smart_dynamic_cast;

SpawnerProperty::SpawnerProperty() :
        m_radius(0.0f), m_minamount(0), m_interval(0)
{
}

SpawnerProperty::~SpawnerProperty()
{
}

void SpawnerProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::TICK_NO, name);

    //Start the tick process by sending an initial tick.
    Anonymous tick_arg;
    tick_arg->setName("spawner");
    Tick t;
    t->setArgs1(tick_arg);
    t->setFutureSeconds(consts::basic_tick * 5.0f);
    t->setTo(owner->getId());
    BaseWorld::instance().message(t, *owner);
}

void SpawnerProperty::apply(LocatedEntity * ent)
{
    auto radius_iter = m_data.find("radius");
    if (radius_iter != m_data.end() && radius_iter->second.isNum()) {
        m_radius = radius_iter->second.Float();
    } else {
        m_radius = 0.0f;
    }

    auto amount_iter = m_data.find("minamount");
    if (amount_iter != m_data.end() && amount_iter->second.isInt()) {
        m_minamount = amount_iter->second.Int();
    } else {
        m_minamount = 0;
    }

    auto type_iter = m_data.find("type");
    if (type_iter != m_data.end() && type_iter->second.isString()) {
        m_type = type_iter->second.String();
    } else {
        m_type = "";
    }

    auto entity_iter = m_data.find("entity");
    if (entity_iter != m_data.end() && entity_iter->second.isMap()) {
        m_entity = entity_iter->second.asMap();
    } else {
        m_entity.clear();
    }

    auto interval_iter = m_data.find("interval");
    if (interval_iter != m_data.end() && interval_iter->second.isNum()) {
        m_interval = interval_iter->second.asNum();
    } else {
        m_interval = 0;
    }
}

HandlerResult SpawnerProperty::operation(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    return tick_handler(e, op, res);
}

SpawnerProperty * SpawnerProperty::copy() const
{
    return new SpawnerProperty(*this);
}

HandlerResult SpawnerProperty::tick_handler(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    if (!op->getArgs().empty()) {
        auto& arg = op->getArgs().front();
        if (arg->getName() == "spawner") {
            //This is our tick
            handleTick(e, op, res);
            return OPERATION_BLOCKED;
        }
    }
    return OPERATION_IGNORED;
}

void SpawnerProperty::handleTick(LocatedEntity * e, const Operation & op,
        OpVector & res)
{
    Anonymous tick_arg;
    tick_arg->setName("spawner");
    Tick t;
    t->setArgs1(tick_arg);
    t->setTo(e->getId());
    if (m_interval == 0) {
        t->setFutureSeconds(consts::basic_tick * 10);
    } else {
        t->setFutureSeconds(m_interval);
    }
    res.push_back(t);

    auto parentLoc = e->m_location.m_loc;
    if (!parentLoc) {
        //If there's no parent entity we should just ignore.
        return;
    }

    if (m_type.empty()) {
        return;
    }

    if (m_minamount == 0) {
        return;
    }

    auto type = Inheritance::instance().getType(m_type);
    if (type == nullptr) {
        return;
    }

    //Check if there are enough entities (with an optional radius)
    float squared_radius = m_radius * m_radius;
    int counter = 0;
    for (auto& entity : *parentLoc->m_contains) {
        if (entity->getType() == type) {
            if (squared_radius == 0
                    || WFMath::SquaredDistance(e->m_location.m_pos,
                            entity->m_location.m_pos) <= squared_radius) {
                counter++;
                if (counter >= m_minamount) {
                    return;
                }
            }
        }
    }

    //If we've come here there's not enough entities of the requested
    //type within the radius; spawn a new one
    createNewEntity(e, op, res);

    return;

}

void SpawnerProperty::createNewEntity(LocatedEntity * e, const Operation & op,
        OpVector & res)
{
    Anonymous create_arg;

    //randomize position and rotation
    WFMath::MTRand& rand = WFMath::MTRand::instance;
    float angle = rand.randf(WFMath::numeric_constants<float>::pi() * 2);
    //place it between 0 and 2 meters away
    float distance = rand.randf(2.0f);
    //if we're solid we should make sure it's not within our own radius
    if (e->m_location.isSolid()) {
        distance += e->m_location.radius();
    }
    //and finally make sure that it's not beyond the radius for checking
    if (m_radius != 0.0f) {
        distance = std::min(m_radius, distance);
    }

    float rotation = rand.randf(WFMath::numeric_constants<float>::pi() * 2);

    float x = (distance * std::cos(angle));
    float y = (distance * std::sin(angle));

    if (!m_entity.empty()) {
        create_arg = smart_dynamic_cast<Anonymous>(
                Factories::instance()->createObject(m_entity));
        if (!create_arg.isValid()) {
            log(ERROR,
                    "Could not parse 'entity' data on spawner into Entity instance.");
            return;
        }
    } else {
        create_arg->setParents(std::list<std::string>(1, m_type));
    }
    create_arg->setLoc(e->m_location.m_loc->getId());
    ::addToEntity(
            WFMath::Point<3>(e->m_location.pos()).shift(
                    WFMath::Vector<3>(x, y, 0)), create_arg->modifyPos());
    WFMath::Quaternion orientation(WFMath::Vector<3>(0, 0, 1), rotation);
    create_arg->setAttr("orientation", orientation.toAtlas());

    Create create;
    create->setTo(e->m_location.m_loc->getId());
    create->setArgs1(create_arg);
    res.push_back(create);

    log(NOTICE, String::compose("Spawner belonging to entity %1 creating new"
            " entity of type %2", e->getId(), m_type));
}

