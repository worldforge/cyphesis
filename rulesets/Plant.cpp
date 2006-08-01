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

#include "Plant.h"

#include "Script.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/Property.h"

#include "common/log.h"

#include "common/Tick.h"
#include "common/Eat.h"
#include "common/Chop.h"
#include "common/Nourish.h"

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

Plant::Plant(const std::string & id, long intId) : Plant_parent(id, intId),
                                                   m_fruits(0),
                                                   m_radius(1),
                                                   m_fruitChance(2),
                                                   m_sizeAdult(4),
                                                   m_nourishment(0)
{
    // Default to a 1m cube
    m_location.setBBox(BBox(WFMath::Point<3>(-0.5, -0.5, 0),
                            WFMath::Point<3>(0.5, 0.5, 1)));

    m_properties["fruits"] = new Property<int>(m_fruits, a_fruit);
    m_properties["radius"] = new Property<int>(m_radius, a_fruit);
    m_properties["fruitName"] = new Property<std::string>(m_fruitName, a_fruit);
    m_properties["fruitChance"] = new Property<int>(m_fruitChance, a_fruit);
    m_properties["sizeAdult"] = new Property<double>(m_sizeAdult, a_fruit);
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

void Plant::ChopOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant got chop op" << std::endl << std::flush;);
    Element mode;
    if (getAttr("mode", mode) && mode.isString() && mode.String() == "felled") {
        debug(std::cout << "Plant is already down" << std::endl << std::flush;);
        Set set_op;
        Anonymous set_arg;
        set_arg->setId(getId());
        set_arg->setAttr("status", -1);
        set_op->setArgs1(set_arg);
        set_op->setTo(getId());
        res.push_back(set_op);

        if (m_location.bBox().isValid()) {
            debug(std::cout << "Plant replaced by log" << std::endl << std::flush;);
            Create create_op;
            Anonymous create_arg;
            create_arg->setParents(std::list<std::string>(1,"lumber"));
            create_arg->setAttr("mass", getMass());
            m_location.addToEntity(create_arg);
            create_op->setArgs1(create_arg);
            create_op->setTo(getId());
            res.push_back(create_op);
        }
        return;
    }
    // FIXME In the future it will take more than one chop to chop down a tree.
    Move move;
    Anonymous move_arg;
    if (m_location.m_loc != NULL) {
        move_arg->setLoc(m_location.m_loc->getId());
    } else {
        log(ERROR, "Plant generating invalid Move op because LOC is NULL");
    }
    Vector3D axis(uniform(-1, 1), uniform(-1, 1), 0);
    axis.normalize();
    // FIXME Make tree fall away from axe, by using cross product of
    // distance to axe, and vertical axis as axis of rotation
    Quaternion orient(m_location.orientation());
    orient.rotation(axis, M_PI/2);
    move_arg->setAttr("orientation", orient.toAtlas());
    move_arg->setAttr("mode", "felled");
    ::addToEntity(m_location.pos(), move_arg->modifyPos());
    move_arg->setId(getId());
    move->setArgs1(move_arg);
    move->setTo(getId());
    res.push_back(move);
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

    double status = m_status;
    if (m_nourishment <= 0) {
        status -= 0.1;
    } else {
        status += 0.1;
        if (status > 1.) {
            status = 1.;
        }
        double new_mass = m_mass + m_nourishment;
        m_nourishment = 0;
        Element maxmass_attr;
        if (getAttr("maxmass", maxmass_attr)) {
            if (maxmass_attr.isNum()) {
                new_mass = std::min(new_mass, maxmass_attr.asNum());
            }
        }
        set_arg->setAttr("mass", new_mass);
        double scale = new_mass / m_mass;
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

    int dropped = dropFruit(res);
    if (m_location.bBox().isValid() && 
        (m_location.bBox().highCorner().z() > m_sizeAdult)) {
        if (randint(1, m_fruitChance) == 1) {
            m_fruits++;
            dropped--;
        }
    }
    if ((dropped != 0) || (m_status < 1.)) {
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
