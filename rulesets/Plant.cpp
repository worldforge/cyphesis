// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#include "Plant.h"

#include "Script.h"
#include "StatusProperty.h"
#include "BBoxProperty.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/compose.hpp"
#include "common/TypeNode.h"
#include "common/Property.h"
#include "common/PropertyManager.h"

#include "common/log.h"

#include "common/Eat.h"
#include "common/Tick.h"
#include "common/Update.h"

#include <wfmath/atlasconv.h>
#include <wfmath/MersenneTwister.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Plant::Plant(const std::string & id, long intId) :
       Identified(id, intId),
       Plant_parent(id, intId), m_nourishment(0)
{
}

Plant::~Plant()
{
}

/// \brief Generate operations to drop a number of fruit
///
/// @return -1 if this plant does not fruit,
/// number of fruit dropped otherwise.
int Plant::dropFruit(OpVector & res, Property<int> * fruits_prop)
{
    Element fruitName;
    if (!getAttrType("fruitName", fruitName, Element::TYPE_STRING)) {
        return -1;
    }
    int & fruits = fruits_prop->data();
    if (fruits < 1) { return 0; }
    int drop = std::min(fruits, randint(m_minuDrop, m_maxuDrop));
    if (drop < 1) {
        return 0;
    }
    fruits -= drop;
    // FIXME apply?
    fruits_prop->setFlags(flag_unsent);

    debug(std::cout << "Dropping " << drop << " fruits from "
                    << m_type << " plant." << std::endl << std::flush;);
    float height = m_location.bBox().highCorner().z(); 
    for(int i = 0; i < drop; ++i) {
        float rx = m_location.pos().x() + uniform( height,
                                                  -height);
        float ry = m_location.pos().y() + uniform( height,
                                                  -height);
        Anonymous fruit_arg;
        fruit_arg->setParents(std::list<std::string>(1, fruitName.String()));
        Location floc(m_location.m_loc, Point3D(rx, ry, 0));
        floc.addToEntity(fruit_arg);
        Create create;
        create->setTo(getId());
        create->setArgs1(fruit_arg);
        res.push_back(create);
    }
    return drop;
}

void Plant::NourishOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Nourish(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    if (op->getArgs().empty()) {
        error(op, "Nourish has no argument", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    Element mass;
    if (arg->copyAttr("mass", mass) != 0 || !mass.isNum()) {
        return;
    }
    m_nourishment += mass.asNum();
    debug(std::cout << "Nourishment: " << m_nourishment
                    << std::endl << std::flush;);
}

void Plant::TickOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Tick(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    // Use a value seeded from the ID, so it's always the same.
    WFMath::MTRand::instance.seed(getIntId());
    float jitter = WFMath::MTRand::instance.rand() * 10;

    Tick tick_op;
    tick_op->setTo(getId());
    tick_op->setFutureSeconds(consts::basic_tick * m_speed + jitter);
    res.push_back(tick_op);

    // FIXME I don't like having to do this test, as its only required
    // during the unit tests.
    // Log an error perhaps?
    // FIXME This causes a character holding an uprooted plant to die.
    if (m_location.m_loc != 0) {
        Eat eat_op;
        eat_op->setTo(m_location.m_loc->getId());
        res.push_back(eat_op);
    }

    // The update op will broadcast notification for all properties that
    // are marked flag_unsent
    Update update;
    update->setTo(getId());
    res.push_back(update);

    StatusProperty * status = requirePropertyClass<StatusProperty>("status");
    double & new_status = status->data();
    status->setFlags(flag_unsent);
    if (m_nourishment <= 0) {
        new_status -= 0.1;
    } else {
        new_status += 0.1;
        if (new_status > 1.) {
            new_status = 1.;
        }

        Property<double> * mass_prop = requirePropertyClass<Property<double> >("mass", 0.);
        double & mass = mass_prop->data();
        double old_mass = mass;
        mass += m_nourishment;
 
        m_nourishment = 0;
        Element maxmass_attr;
        if (getAttrType("maxmass", maxmass_attr, Element::TYPE_FLOAT)) {
            mass = std::min(mass, maxmass_attr.Float());
        }
        PropertyBase * biomass = getProperty("biomass");
        if (biomass != 0) {
            if (biomass->flags() & flag_class) {
                m_properties["biomass"] = biomass = PropertyManager::instance()->addProperty("biomass", Element::TYPE_FLOAT);
            }
            biomass->set(mass);
            biomass->setFlags(flag_unsent);
        }

        BBox & bbox = m_location.m_bBox;
        // FIXME Handle the bbox without needing the Set operation.
        if (old_mass != 0 && bbox.isValid()) {
            double scale = mass / old_mass;
            double height_scale = pow(scale, 0.33333f);
            debug(std::cout << "scale " << scale << ", " << height_scale
                            << std::endl << std::flush;);
            debug(std::cout << "Old " << bbox << std::endl << std::flush;);
            bbox = BBox(Point3D(bbox.lowCorner().x() * height_scale,
                                bbox.lowCorner().y() * height_scale,
                                bbox.lowCorner().z() * height_scale),
                        Point3D(bbox.highCorner().x() * height_scale,
                                bbox.highCorner().y() * height_scale,
                                bbox.highCorner().z() * height_scale));
            debug(std::cout << "New " << bbox << std::endl << std::flush;);
            BBoxProperty * box_property = getPropertyClass<BBoxProperty>("biomass");
            if (box_property != 0) {
                box_property->data() = bbox;
            } else {
                log(ERROR, String::compose("Plant %1 type \"%2\" has a valid "
                                           "bbox, but no bbox property",
                                           getIntId(), getType()->name()));
            }
        }
    }

    Property<int> * fruits_prop = getPropertyType<int>("fruits");
    if (fruits_prop != 0 && dropFruit(res, fruits_prop) != -1) {
        int & fruits = fruits_prop->data();
        Element fruitChance;
        Element sizeAdult;
        int change = 0;
        if (getAttrType("fruitChance", fruitChance, Element::TYPE_INT) &&
            getAttrType("sizeAdult", sizeAdult, Element::TYPE_FLOAT) &&
            m_location.bBox().isValid() && 
            (m_location.bBox().highCorner().z() > sizeAdult.Float())) {
            if (randint(1, fruitChance.Int()) == 1) {
                fruits++;
                fruits_prop->set(fruits);
                fruits_prop->setFlags(flag_unsent);
            }
        }
    }
}

void Plant::TouchOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Touch(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    debug(std::cout << "Checking for drop"
                    << std::endl << std::flush;);
    Property<int> * fruits_prop = getPropertyType<int>("fruits");
    if (fruits_prop != 0 && dropFruit(res, fruits_prop) > 0) {
        Update update;
        update->setTo(getId());
        res.push_back(update);
    }
}
