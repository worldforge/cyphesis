// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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



#include "physics/Shape.h"

#include "AreaProperty.h"
#include "BBoxProperty.h"

#include "rulesets/MetabolizingProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/LocatedEntity.h"

#include "common/const.h"
#include "common/debug.h"
#include "common/Tick.h"
#include "common/TypeNode.h"
#include "common/Update.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h> // why do I need this ?
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Root;
using Atlas::Message::Element;

static const bool debug_flag = true;

static const std::string GROWS = "grows";
static const std::string MAXMASS = "maxmass";
static const std::string MASS = "mass";
static const std::string MASSRESERVE = "massreserve";
static const std::string NOURISHMENT = "nourishment";
static const std::string RESERVELIMIT = "reservelimit";
static const std::string STATUS = "status";

// This the amount of energy consumed each tick
const double MetabolizingProperty::energyUnitBase = 0.0001;

// How much energy we get burning one unit of mass
const double MetabolizingProperty::energyToMass = 10.0;

// How much energy creature is able to store as mass (relative to mass) 
// (used if no reserveLimit provided)
const double MetabolizingProperty::defaultReserves = 0.5;

// How much mass creature is able to eat per tick, relative to creature's own mass
const double MetabolizingProperty::biteSizeBase = 0.01;



void MetabolizingProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::TICK_NO, name);

    // Start the metabolize tick loop
    Tick metabolizeOp;
    metabolizeOp->setTo(owner->getId());
    metabolizeOp->setFrom(owner->getId());

    Anonymous metabolize_arg;
    metabolize_arg->setName("metabolize");
    metabolizeOp->setArgs1(metabolize_arg);

    //FIXME i hope i'm doing it right ....
    owner->sendWorld(metabolizeOp);
}

void MetabolizingProperty::remove(LocatedEntity *owner, const std::string & name)
{
    owner->removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

HandlerResult MetabolizingProperty::operation(LocatedEntity * e,
                                         const Operation & op,
                                         OpVector & res)
{
    return tick_handler(e, op, res);
}

MetabolizingProperty * MetabolizingProperty::copy() const
{
    return new MetabolizingProperty(*this);
}

/// \brief Calculate how the Characters metabolism has affected it in the
/// last tick
///
/// This function is called every tick (1 per 90 seconds) It does one of three things.
/// If energy is very high, it loses some, and gains some weight. Otherwise
/// it loses some energy, unless energy is very low, in which case loss
/// is slower, as weight is used to compensate.
/// A fully healthy Character should take about a week to starve to death.
/// So 10080 / 90 = 6720 ticks.
/// @param res Any result of changes is returned here.
HandlerResult MetabolizingProperty::tick_handler(LocatedEntity * e,
                                           const Operation & op,
                                           OpVector & res)
{
 
    debu(std::cout << "MetabolizingProperty::tick_handler(entId="
                    << e->getId() << ")" <<std::endl << std::flush;);

    // Check if this Tick operation is meant to trigger "metabolism tick"
    const std::vector<Root> & args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();
        if (arg->getName() != "metabolize") {
            return OPERATION_IGNORED;
        }
    }
    else { 
        return OPERATION_IGNORED;
    }

    // atm the metabolizng property has interpretation of metabolism speed
    // Should be 1 for human, around 2 for wolf and 0.9 for deer
    // faster metabolism increase both status regeneration and status usage
    double & metabolizingSpeed = data(); 
    double energyUnit = energyUnitBase * metabolizingSpeed;    

    StatusProperty * status_prop = e->requirePropertyClass<StatusProperty>(STATUS, 1.0f);
    bool status_changed = false;
    assert(status_prop != 0);
    status_prop->setFlags(flag_unsent);
    double & status = status_prop->data();


    // get max mass reserves
    const Property<double> * reservelimit_prop = e->getPropertyType<double>(RESERVELIMIT);
    double reserveLimit = defaultReserves;
    if (reservelimit_prop != 0) {
        reserveLimit = reservelimit_prop->data();
    }
  
    // get mass property
    Property<double> * mass_prop = e->modPropertyType<double>(MASS);
 
    // get mass reserves (new property introduced for metabolism to work)
    Property<double> * massreserve_prop; 
    if (mass_prop != 0) {
        double massreserve = mass_prop->data() * reserveLimit * 0.5;
        massreserve_prop = e->requirePropertyClass<Property<double> >(MASSRESERVE, massreserve);
        massreserve_prop->setFlags(flag_unsent);
    }
    else {
        massreserve_prop = nullptr; // FIXME how to tell that its null pointer
    }


    // get nourishment property
    Property<double> * nourishment_prop = e->modPropertyType<double>(NOURISHMENT); 
    // TODO later we should use unified nourishment property instead of nourishment
    
    double nourishmentConsumed = 0;
    double biteSize = biteSizeBase * metabolizingSpeed;
    
    // DIGESTION
    // don't go further if we don't have following properties
    if (nourishment_prop != 0 && mass_prop != 0) {

        double & nourishment = nourishment_prop->data();
        double & mass = mass_prop->data();

        // set the nourishment bite size depending on creature size and metabolizingSpeed
        nourishmentConsumed = mass*biteSize;
        if (nourishment >= nourishmentConsumed) {
            nourishment -= nourishmentConsumed;

            nourishment_prop->setFlags(flag_unsent);
            nourishment_prop->apply(e);
            status += biteSize*energyToMass;
            status_changed = true;
        }
        else {
            nourishmentConsumed = 0;
        }
    }

    // Default growth factor (1- means no growth)
    float growth = 1.0f; 

    // Status should be between 0 and ?
    // WEIGHT GAIN for high status
    if (status > (1.5 + energyUnit)) {
        status -= energyUnit;
        status_changed = true;
        if (mass_prop != 0 && massreserve_prop != 0 ) {
            double & mass = mass_prop->data();
            double & massReserve = massreserve_prop->data();
            double massChange = mass*energyUnit/energyToMass;

            // when calculating growth take into account only mass that is no reserve
            // growth factor is always equal to new core mass divided by old one
            double coreMass = mass-massReserve;

            Element maxmass_attr;
            if (e->getAttrType(MAXMASS, maxmass_attr, Element::TYPE_FLOAT) == 0) {
                massChange = std::min(massChange, maxmass_attr.Float()-mass);
            }
            // TODO 
            // Change the mass reserves depending on how fat we are
            //
            // we have small reserves put all mass gain into it
            // no growth in size in this case
            if (massReserve < mass*0.5*reserveLimit) {
                // growth factor
                growth = 1.0f + (1.0f - reserveLimit)*massChange/coreMass;
              
                massReserve += reserveLimit * massChange;
            }
            // normal reserves -> grow a little
            else if (massReserve < mass*reserveLimit) {
                // growth factor
                growth = 1.0f + (1.0f - 0.5*reserveLimit)*massChange/coreMass; 
              
                massReserve += massChange*reserveLimit * 0.5;
            } 
            // other cases max mass reserve
            else {
                // growth factor
                growth = 1.0f + massChange/coreMass; 

                massReserve = massChange*reserveLimit;
            }
            mass += massChange;

            mass_prop->setFlags(flag_unsent);
            massreserve_prop->setFlags(flag_unsent);
            mass_prop->apply(e);
            massreserve_prop->apply(e);
        }
    }
    // increase energy, no mass growth - no growth
    else {
        status -= energyUnit;
        status_changed = true;
        if (mass_prop != 0 && massreserve_prop != 0) {
            double & mass = mass_prop->data();
            double & massReserve = massreserve_prop->data();
            double massBurn = mass*energyUnit/energyToMass;
            if (status <= 0.5 && massReserve > massBurn) {
                // all of the burned mass comes from the reserves, 
                // entity should die if it lose all its mass reserves
                status += energyUnit;
                status_changed = true;
                mass -= massBurn;
                massReserve -= massBurn;
                mass_prop->setFlags(flag_unsent);
                massreserve_prop->setFlags(flag_unsent);
                mass_prop->apply(e);
                massreserve_prop->apply(e);
            }
        }
    }

    if (status_changed) {
        status_prop->apply(e);
    }

    if (mass_prop != 0 && massreserve_prop != 0) {
        debug(std::cout << "MetabolizingProperty::tick_handler(entId="
                    << e->getId() << ") nourishmentConsumed: " << nourishmentConsumed 
                    << " massReserve: " << massreserve_prop->data()
                    << " reserveLimit: " << reserveLimit
                    << " status: " << status
                    << " mass: " << mass_prop->data()
                    << std::endl << std::flush;);
    }


    // Decide wether enity should grow
    const Property<int> * grows_prop = e->getPropertyType<int>(GROWS);
    if (grows_prop != 0) {
        if (growth != 1.0f && grows_prop->data() > 0 ) {
          grow(e, growth);
        }
    }


    Update update;              
    update->setTo(e->getId());

    res.push_back(update);

    //Issue new metabolize tick
    Tick metabolizeOp;
    metabolizeOp->setTo(e->getId());
    Anonymous metabolize_arg;
    metabolize_arg->setName("metabolize");
    metabolizeOp->setArgs1(metabolize_arg);
    // this should remain fixed, we should scale metabolism speed using other means
    metabolizeOp->setFutureSeconds(consts::basic_tick * 30); 
    res.push_back(metabolizeOp);

    return OPERATION_BLOCKED;
}


void MetabolizingProperty::grow(LocatedEntity * e, float scale) {
 
    debug(std::cout << "Entity: " << e->getIntId()  << " grows by factor: " 
                        << scale << ", entity is of a type: " << e->getType()->name() 
                        << std::endl << std::flush;);

    
    BBox & bbox = e->m_location.m_bBox;
    // FIXME Handle the bbox without needing the Set operation.
    if (scale > 0 && bbox.isValid()) {
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
        BBoxProperty * box_property = e->modPropertyClass<BBoxProperty>("bbox");
        if (box_property) {
            box_property->data() = bbox;
            box_property->setFlags(flag_unsent);
        } else {
            log(ERROR, String::compose("Entity %1 type \"%2\" has a valid "
                "bbox, but no bbox property",
                e->getIntId(), e->getType()->name()));
        }

        // scale area property (if exists)
        scaleArea(e);
       
    }
    // NOTE for those changes to have an effect one must send update operation later
}

void MetabolizingProperty::scaleArea(LocatedEntity * e) {
    static float AREA_SCALING_FACTOR=3.0f;

    const WFMath::AxisBox<3>& bbox = e->m_location.bBox();
    if (bbox.isValid()) {
        //If there's an area we need to scale that with the bbox
        AreaProperty * area_property = e->modPropertyClass<AreaProperty>("area");
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
                    area_property->setFlags(flag_unsent);
                }
            }
        }
    }
    // This function should be executed before update operation is issued
}
