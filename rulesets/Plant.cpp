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

#include "StatusProperty.h"
#include "BBoxProperty.h"
#include "AreaProperty.h"
#include "physics/Shape.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/compose.hpp"
#include "common/TypeNode.h"
#include "common/Property.h"

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
       Thing(id, intId)
{
}

Plant::~Plant()
{
}

/// \brief Generate operations to drop a fruit.
///
void Plant::dropFruit(OpVector & res, const std::string& fruitName)
{

    debug(std::cout << "Dropping a fruit from "
                    << m_type << " plant." << std::endl << std::flush;);
    float height = m_location.bBox().highCorner().z();
    float rx = m_location.pos().x() + uniform( height,
                                              -height);
    float ry = m_location.pos().y() + uniform( height,
                                              -height);
    Anonymous fruit_arg;
    fruit_arg->setParents(std::list<std::string>(1, fruitName));
    Location floc(m_location.m_loc, Point3D(rx, ry, 0));
    floc.addToEntity(fruit_arg);
    Create create;
    create->setTo(getId());
    create->setArgs1(fruit_arg);
    res.push_back(create);
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
    if (!m_nourishment) {
        m_nourishment = mass.asNum();
    } else {
        *m_nourishment += mass.asNum();
    }
    debug(std::cout << "Nourishment: " << m_nourishment
                    << std::endl << std::flush;);
}

void Plant::TickOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Plant::Tick(" << getId() << "," << m_type << ")"
                    << std::endl << std::flush;);
    // Use a value seeded from the ID, so it's always the same.
    WFMath::MTRand::instance.seed(getIntId());
    double jitter = WFMath::MTRand::instance.rand() * 10.;

    Tick tick_op;
    tick_op->setTo(getId());
    tick_op->setFutureSeconds(consts::basic_tick * m_speed + jitter);
    res.push_back(tick_op);

    // The update op will broadcast notification for all properties that
    // are marked flag_unsent
    Update update;
    update->setTo(getId());
    res.push_back(update);

    //Only do nourishment check if we've had a chance to send an Eat op.
    //Else we'll be shrinking each time the server is restarted.
    if (m_nourishment) {
        StatusProperty * status = requirePropertyClass<StatusProperty>("status", 1);
        double & new_status = status->data();
        status->setFlags(flag_unsent);
        if (*m_nourishment <= 0) {
            debug(std::cout << "No nourishment; shrinking."
                            << std::endl << std::flush;);
            new_status -= 0.1;
        } else {
            new_status += 0.1;
            if (new_status > 1.) {
                new_status = 1.;
            }

            Property<double> * mass_prop = requirePropertyClass<Property<double> >("mass", 0.);
            double & mass = mass_prop->data();
            double old_mass = mass;
            mass += *m_nourishment;

            *m_nourishment = 0;
            Element maxmass_attr;
            if (getAttrType("maxmass", maxmass_attr, Element::TYPE_FLOAT) == 0) {
                mass = std::min(mass, maxmass_attr.Float());
            }
            PropertyBase * biomass = modPropertyType<double>("biomass");
            if (biomass != nullptr) {
                biomass->set(mass);
                biomass->setFlags(flag_unsent);
            }
            //TODO: we need to sort out how to handle mass and biomass
            mass_prop->set(mass);
            mass_prop->setFlags(flag_unsent);

            BBox & bbox = m_location.m_bBox;
            // FIXME Handle the bbox without needing the Set operation.
            if (old_mass != 0 && bbox.isValid()) {
                float scale = (float)(mass / old_mass);
                float height_scale = std::pow(scale, 0.33333f);
                debug(std::cout << "scale " << scale << ", " << height_scale
                                << std::endl << std::flush;);
                debug(std::cout << "Old " << bbox << std::endl << std::flush;);
                // FIXME Rammming in a bbox without checking if its valid.
                bbox = BBox(Point3D(bbox.lowCorner().x() * height_scale,
                                    bbox.lowCorner().y() * height_scale,
                                    bbox.lowCorner().z() * height_scale),
                            Point3D(bbox.highCorner().x() * height_scale,
                                    bbox.highCorner().y() * height_scale,
                                    bbox.highCorner().z() * height_scale));
                debug(std::cout << "New " << bbox << std::endl << std::flush;);
                BBoxProperty * box_property = modPropertyClass<BBoxProperty>("bbox");
                if (box_property != nullptr) {
                    box_property->data() = bbox;
                    box_property->apply(this);
                    box_property->setFlags(flag_unsent);
                } else {
                    log(ERROR, String::compose("Plant %1 type \"%2\" has a valid "
                                               "bbox, but no bbox property",
                                               getIntId(), getType()->name()));
                }

                scaleArea();

            }
        }
        status->apply(this);
    }

    // FIXME I don't like having to do this test, as its only required
    // during the unit tests.
    // Log an error perhaps?
    // FIXME This causes a character holding an uprooted plant to die.
    if (m_location.m_loc != nullptr) {
        Eat eat_op;
        eat_op->setTo(m_location.m_loc->getId());
        res.push_back(eat_op);
        //Initialize nourishment to zero once we've sent our first Eat op.
        if (!m_nourishment) {
            m_nourishment = .0;
        }
    }


    //Only handle fruits if the plant is of adult size.
    Property<int> * fruits_prop = modPropertyType<int>("fruits");
    if (fruits_prop != nullptr) {
        Element sizeAdult;
        if (getAttrType("sizeAdult", sizeAdult, Element::TYPE_FLOAT) == 0 ||
                getAttrType("sizeAdult", sizeAdult, Element::TYPE_INT) == 0) {
            //Only drop fruits if we're an adult
            if (m_location.bBox().isValid() &&
                    (m_location.bBox().highCorner().z() >= sizeAdult.asNum())) {
                handleFruiting(res, *fruits_prop);
            }
        }
    }
}

void Plant::handleFruiting(OpVector & res, Property<int>& fruits_prop) {
    Element fruitName;
    if (getAttrType("fruitName", fruitName, Element::TYPE_STRING) != 0) {
        return;
    }

    auto& fruits = fruits_prop.data();
    Element fruitsChance;
    if (getAttrType("fruitChance", fruitsChance, Element::TYPE_INT) == 0) {
        //First check if we should drop fruits.
        if (fruits > 0) {
            //TODO: use a different attribute than fruitChance for this
            if (randint(0, 100) < fruitsChance.Int()) {
                fruits--;
                fruits_prop.setFlags(flag_unsent);
                dropFruit(res, fruitName.String());
            }
        }


        //Then see if we should increase the number of fruits.
        Element fruitsMax;
        //Increase fruits if there's either no max value, or we haven't reached it yet.
        if (getAttrType("fruitsMax", fruitsMax, Element::TYPE_INT) != 0 || fruitsMax.Int() > fruits_prop.data()) {
            //FruitChance is between [0..100] (percentage).
            if (randint(0, 100) < fruitsChance.Int()) {
                fruits++;
                fruits_prop.setFlags(flag_unsent);
            }
        }
    }
}

void Plant::TouchOperation(const Operation & op, OpVector & res)
{
    Element sizeAdult;
    if (getAttrType("sizeAdult", sizeAdult, Element::TYPE_FLOAT) == 0
            || getAttrType("sizeAdult", sizeAdult, Element::TYPE_INT) == 0) {
        //Only drop fruits if we're an adult
        if (m_location.bBox().isValid()
                && (m_location.bBox().highCorner().z() >= sizeAdult.asNum())) {

            Property<int> * fruits_prop = modPropertyType<int>("fruits");
            if (fruits_prop != nullptr) {
                if (fruits_prop->data() <= 0) {
                    return;
                }
                Element fruitName;
                if (getAttrType("fruitName", fruitName, Element::TYPE_STRING) != 0) {
                    return;
                }
                Element fruitsChance;
                if (getAttrType("fruitChance", fruitsChance, Element::TYPE_INT) != 0) {
                    return;
                }
                //TODO: use a different attribute than fruitChance for this
                if (randint(0, 100) < fruitsChance.Int()) {
                    fruits_prop->data()--;
                    fruits_prop->setFlags(flag_unsent);
                    dropFruit(res, fruitName.String());

                    Update update;
                    update->setTo(getId());
                    res.push_back(update);
                }

            }
        }
    }
}


void Plant::scaleArea() {
    static float AREA_SCALING_FACTOR=3.0f;

    const WFMath::AxisBox<3>& bbox = m_location.bBox();
    if (bbox.isValid()) {
        //If there's an area we need to scale that with the bbox
        AreaProperty * area_property = modPropertyClass<AreaProperty>("area");
        if (area_property != nullptr) {
            WFMath::AxisBox<2> footprint = area_property->shape()->footprint();
            //We'll make it so that the footprint of the area is AREA_SCALING_FACTOR times the footprint of the bbox
            auto area_radius = footprint.boundingSphere().radius();
            if (area_radius != 0.0f) {

                //We're only interested in the horizontal radius of the plant
                WFMath::AxisBox<2> flat_bbox(WFMath::Point<2>(bbox.lowerBound(0), bbox.lowerBound(1)), WFMath::Point<2>(bbox.upperBound(0), bbox.upperBound(1)));
                auto plant_radius = flat_bbox.boundingSphere().radius();

                auto desired_radius = plant_radius * AREA_SCALING_FACTOR;
                auto scaling_factor = desired_radius / area_radius;

                //No need to alter if the scale is the same.
                //Also don't scale the unless the difference is at least 10% in either direction.
                //The reason for this is that we don't want to alter the area each tick since
                //the client often must perform a sometimes expensive material regeneration
                //calculation every time a terrain area changes. With many plants this runs the
                //risk of bogging down the client then.
                if (!WFMath::Equal(scaling_factor, 1.0f)
                        && (scaling_factor > 1.1f || scaling_factor < 0.9f)) {
                    std::unique_ptr<Form<2>> new_area_shape(
                            area_property->shape()->copy());
                    new_area_shape->scale(scaling_factor);
                    Atlas::Message::MapType shapeElement;
                    new_area_shape->toAtlas(shapeElement);

                    Atlas::Message::Element areaElement;
                    area_property->get(areaElement);
                    areaElement.asMap()["shape"] = shapeElement;

                    area_property->set(areaElement);
                    area_property->apply(this);
                    area_property->setFlags(flag_unsent);
                }
            }
        }
    }
}
