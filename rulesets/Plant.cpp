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

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/Property.h"

#include "common/log.h"

#include "common/Tick.h"
#include "common/Eat.h"

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
/// 0 if number of fruit has not changes,
/// 1 if number of fruit has changed.
int Plant::dropFruit(OpVector & res, int & fruits)
{
    Element fruitName;
    if (!getAttr("fruitName", fruitName) || !fruitName.isString()) {
        return -1;
    }
    Element fruitsAttr;
    if (!getAttr("fruits", fruitsAttr) || !fruitsAttr.isInt()) {
        fruits = 0;
        return 1;
    }
    fruits = fruitsAttr.Int();
    if (fruits < 1) { return 0; }
    int drop = std::min(fruits, randint(m_minuDrop, m_maxuDrop));
    if (drop < 1) {
        return 0;
    }
    fruits -= drop;
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
    return 1;
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

void Plant::SetupOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Setup(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    Tick tick;
    tick->setTo(getId());

    res.push_back(tick);
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

    Set set_op;
    set_op->setTo(getId());
    res.push_back(set_op);

    Anonymous set_arg;
    set_arg->setId(getId());

    Element new_status(1.);
    PropertyBase * status = getProperty("status");
    if (status != 0) {
        status->get(new_status);
    }
    assert(new_status.isFloat());
    if (m_nourishment <= 0) {
        new_status = new_status.Float() - 0.1;
    } else {
        new_status = new_status.Float() + 0.1;
        if (new_status.Float() > 1.) {
            new_status = 1.;
        }
        set_arg->setAttr("status", new_status);

        Element mass_attr;
        double mass = 0;
        if (getAttr("mass", mass_attr) && mass_attr.isFloat()) {
            mass = mass_attr.Float();
        }
        double new_mass = mass + m_nourishment;
        m_nourishment = 0;
        Element maxmass_attr;
        if (getAttr("maxmass", maxmass_attr)) {
            if (maxmass_attr.isNum()) {
                new_mass = std::min(new_mass, maxmass_attr.asNum());
            }
        }
        set_arg->setAttr("mass", new_mass);
        if (hasAttr("biomass")) {
            set_arg->setAttr("biomass", new_mass);
        }

        if (mass != 0) {
            double scale = new_mass / mass;
            double height_scale = pow(scale, 0.33333f);
            debug(std::cout << "scale " << scale << ", " << height_scale
                            << std::endl << std::flush;);
            const BBox & ob = m_location.bBox();
            BBox new_bbox(Point3D(ob.lowCorner().x() * height_scale,
                                  ob.lowCorner().y() * height_scale,
                                  ob.lowCorner().z() * height_scale),
                          Point3D(ob.highCorner().x() * height_scale,
                                  ob.highCorner().y() * height_scale,
                                  ob.highCorner().z() * height_scale));
            debug(std::cout << "Old " << ob
                            << "New " << new_bbox << std::endl << std::flush;);
            set_arg->setAttr("bbox", new_bbox.toAtlas());
        }
    }

    int fruits;
    int change = dropFruit(res, fruits);
    if (change != -1) {
        Element fruitChance;
        Element sizeAdult;
        if (getAttr("fruitChance", fruitChance) && fruitChance.isInt() &&
            getAttr("sizeAdult", sizeAdult) && sizeAdult.isNum() &&
            m_location.bBox().isValid() && 
            (m_location.bBox().highCorner().z() > sizeAdult.asNum())) {
            if (randint(1, fruitChance.Int()) == 1) {
                change = 1;
                fruits++;
            }
        }
        if (change != 0) {
            set_arg->setAttr("fruits", fruits);
        }
    }
    set_op->setArgs1(set_arg);
}

void Plant::TouchOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Touch(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    debug(std::cout << "Checking for drop"
                    << std::endl << std::flush;);
    int fruits;
    int chg = dropFruit(res, fruits);
    debug(std::cout << "Plant has " << fruits << " fruits right now"
                    << std::endl << std::flush;);
    if (chg > 0) {
        Set set;
        Anonymous set_arg;
        set_arg->setId(getId());
        set_arg->setAttr("fruits", fruits);
        set->setTo(getId());
        set->setArgs1(set_arg);
        res.push_back(set);
    }
}
