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
#include "DensityProperty.h"
#include "Vector3Property.h"
#include "physics/Shape.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/random.h"
#include "common/TypeNode.h"

#include "common/Eat.h"
#include "common/Tick.h"
#include "common/Update.h"

#include <wfmath/MersenneTwister.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Eat;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

static const std::string NOURISHMENT = "nourishment";

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
    fruit_arg->setParent(fruitName);
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
    Element mass_attr;
    if (arg->copyAttr("mass", mass_attr) != 0 || !mass_attr.isNum()) {
        return;
    }

    // Set nourishment property
    Property<double> * nourishment_prop = requirePropertyClass<Property<double> >(NOURISHMENT, 0.f);
    double & nourishment = nourishment_prop->data();
    nourishment += mass_attr.asNum();
    nourishment_prop->setFlags(flag_unsent);

    // FIXME This will become a Update once private properties are sorted
    Anonymous nourishment_ent;
    nourishment_ent->setId(getId());
    nourishment_ent->setAttr(NOURISHMENT, nourishment);
    
    Set s;
    s->setArgs1(nourishment_ent);

    // FIXME HELP Why do I need to send sight operation?? 
    // It seems I require for tests to work but I don't know what the logic behind it
    Sight si;
    si->setTo(getId());
    si->setArgs1(s);
    res.push_back(si);

    debug(std::cout << "Nourishment: " << nourishment
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


    const Property<double> * nourishment_prop = getPropertyType<double>(NOURISHMENT);

    //Only do nourishment check if we've had a chance to send an Eat op.
    //Else we'll be shrinking each time the server is restarted.
    if (nourishment_prop != 0) {
        const double & nourishment = nourishment_prop->data();
        // all mass & status change was moved to metabolise tick
        if (nourishment <= 0) {
            debug(std::cout << "No nourishment; shrinking."
                            << std::endl << std::flush;);
        } else {
            // TODO Handle this in metabolism too
            Property<double> * mass_prop = requirePropertyClass<Property<double> >("mass", 0.);
            PropertyBase * biomass = modPropertyType<double>("biomass");
            BBoxProperty * box_property = requirePropertyClass<BBoxProperty>("bbox");
            BBox & bbox = m_location.m_bBox;
            double & mass = mass_prop->data();
	    /*
<<<<<<< HEAD
            PropertyBase * biomass = modPropertyType<double>("biomass");
            if (biomass != nullptr) {
                biomass->set(mass);
                biomass->setFlags(flag_unsent);
            }
=======
            */
            double old_mass = mass;

            const DensityProperty* densityProperty = getPropertyClassFixed<DensityProperty>();
            if (densityProperty) {
                //There's a density property; we shouldn't change mass directly, instead we should change the size of the entity.
                double newMass = mass + *m_nourishment;

                //Check if there's a maxsize prop, otherwise check with maxmass
                const Vector3Property* maxSizeProp = getPropertyClass<Vector3Property>("maxsize");
                if (maxSizeProp && maxSizeProp->data().isValid() && bbox.isValid() && densityProperty->data() != 0) {
                    WFMath::Vector<3> volumeVector = bbox.highCorner() - bbox.lowCorner();
                    float volume = volumeVector.x() * volumeVector.y() * volumeVector.z();
                    float volumeNew = newMass / densityProperty->data();
                    float scale = volumeNew / volume;
                    boxScale(bbox, scale);

                    //We've scaled the bbox; now check if it exceeds the max size.
                    //0 is ignored.
                    WFMath::Vector<3> newSize = bbox.highCorner() - bbox.lowCorner();
                    const WFMath::Vector<3>& maxSize = maxSizeProp->data();
                    scale = 1.0f;
                    if (maxSize.x() != 0 && newSize.x() > maxSize.x()) {
                        scale = std::min(scale, maxSize.x() / newSize.x());
                    }
                    if (maxSize.y() != 0 && newSize.y() > maxSize.y()) {
                        scale = std::min(scale, maxSize.y() / newSize.y());
                    }
                    if (maxSize.z() != 0 && newSize.z() > maxSize.z()) {
                        scale = std::min(scale, maxSize.z() / newSize.z());
                    }

                    //New box needs to be scaled again to fit with max size.
                    if (scale != 1.0f) {
                        boxScale(bbox, scale);
                    }

                    box_property->data() = bbox;
                    box_property->apply(this);
                    box_property->setFlags(flag_unsent);

                    scaleArea();
                } else {
                    Element maxmass_attr;
                    if (getAttrType("maxmass", maxmass_attr, Element::TYPE_FLOAT) == 0) {
                        newMass = std::min(newMass, maxmass_attr.Float());
                    }

                    if (old_mass != 0 && bbox.isValid()) {
                        float scale = (float)(newMass / old_mass);
                        float height_scale = std::pow(scale, 0.33333f);
                        debug(std::cout << "scale " << scale << ", " << height_scale
                                        << std::endl << std::flush;);
                        debug(std::cout << "Old " << bbox << std::endl << std::flush;);
                        boxScale(bbox, scale);
                        debug(std::cout << "New " << bbox << std::endl << std::flush;);

                        box_property->data() = bbox;
                        box_property->apply(this);
                        box_property->setFlags(flag_unsent);

                        scaleArea();
                    }
                }



                if (biomass != nullptr) {
                    biomass->set(mass);
                    biomass->setFlags(flag_unsent);
                }

            } else {
                mass += *m_nourishment;

                Element maxmass_attr;
                if (getAttrType("maxmass", maxmass_attr, Element::TYPE_FLOAT) == 0) {
                    mass = std::min(mass, maxmass_attr.Float());
                }
                if (biomass != nullptr) {
                    biomass->set(mass);
                    biomass->setFlags(flag_unsent);
                }
                //TODO: we need to sort out how to handle mass and biomass
                mass_prop->set(mass);
                mass_prop->setFlags(flag_unsent);

                // FIXME Handle the bbox without needing the Set operation.
                if (old_mass != 0 && bbox.isValid()) {
                    float scale = (float)(mass / old_mass);

                    debug(std::cout << "Old " << bbox << std::endl << std::flush;);
                    boxScale(bbox, scale);
                    debug(std::cout << "New " << bbox << std::endl << std::flush;);

                    box_property->data() = bbox;
                    box_property->apply(this);
                    box_property->setFlags(flag_unsent);

                    scaleArea();
                }
            }
            *m_nourishment = 0;
//>>>>>>> master
        }
    }

    if (m_location.m_loc != nullptr) {
        Element mode_attr;
        if (getAttrType("mode", mode_attr, Element::TYPE_STRING) == 0 && mode_attr.String() == "planted") {
        	//Only send eat ops if we're planted.
        	Eat eat_op;
            eat_op->setTo(m_location.m_loc->getId());
            res.push_back(eat_op);
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
 
    // The update op will broadcast notification for all properties that
    // are marked flag_unsent
    Update update;
    update->setTo(getId());
    res.push_back(update);
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
