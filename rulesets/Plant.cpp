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

// $Id: Plant.cpp,v 1.87 2008-08-21 17:10:39 alriddoch Exp $

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
       Plant_parent(id, intId),
                                                   m_fruits(0),
                                                   m_radius(1),
                                                   m_fruitChance(2),
                                                   m_sizeAdult(4),
                                                   m_nourishment(0)
{
    // Default to a 1m cube
    m_location.setBBox(BBox(WFMath::Point<3>(-0.5, -0.5, 0),
                            WFMath::Point<3>(0.5, 0.5, 1)));

    m_properties["fruits"] = new Property<int>(m_fruits, 0);
    m_properties["radius"] = new Property<int>(m_radius, 0);
    m_properties["fruitName"] = new Property<std::string>(m_fruitName, 0);
    m_properties["fruitChance"] = new Property<int>(m_fruitChance, 0);
    m_properties["sizeAdult"] = new Property<double>(m_sizeAdult, 0);
}

Plant::~Plant()
{
}

int Plant::dropFruit(OpVector & res)
{
    if (m_fruits < 1) { return 0; }
    if (m_fruitName.empty()) { return 0; }
    int drop = std::min(m_fruits, randint(m_minuDrop, m_maxuDrop));
    m_fruits -= drop;
    debug(std::cout << "Dropping " << drop << " fruits from "
                    << m_type << " plant." << std::endl << std::flush;);
    float height = m_location.bBox().highCorner().z(); 
    for(int i = 0; i < drop; ++i) {
        float rx = m_location.pos().x() + uniform( height * m_radius,
                                                  -height * m_radius);
        float ry = m_location.pos().y() + uniform( height * m_radius,
                                                  -height * m_radius);
        Anonymous fruit_arg;
        fruit_arg->setName(m_fruitName);
        fruit_arg->setParents(std::list<std::string>(1,m_fruitName));
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

void Plant::SetupOperation(const Operation & op, OpVector & res)
{
    Tick tick;
    tick->setTo(getId());

    res.push_back(tick);
}

void Plant::TickOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Tick(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    Tick tick_op;
    tick_op->setTo(getId());
    tick_op->setFutureSeconds(consts::basic_tick * m_speed);
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

    int dropped = dropFruit(res);
    if (m_location.bBox().isValid() && 
        (m_location.bBox().highCorner().z() > m_sizeAdult)) {
        if (randint(1, m_fruitChance) == 1) {
            m_fruits++;
            dropped--;
        }
    }
    if (dropped != 0 || new_status.Float() < 1.) {
        set_arg->setAttr("fruits", m_fruits);
    }
    set_op->setArgs1(set_arg);
}

void Plant::TouchOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Touch(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    debug(std::cout << "Plant has " << m_fruits << " fruits right now"
                    << std::endl << std::flush;);
    debug(std::cout << "Checking for drop"
                    << std::endl << std::flush;);

    int dropped = dropFruit(res);
    if (dropped != 0) {
        Set set;
        Anonymous set_arg;
        set_arg->setId(getId());
        set_arg->setAttr("fruits", m_fruits);
        set->setTo(getId());
        set->setArgs1(set_arg);
        res.push_back(set);
    }
}
