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


#include "rulesets/MetabolizingProperty.h"
#include "rulesets/StatusProperty.h"

#include "rulesets/LocatedEntity.h"

#include "common/debug.h"

#include "common/Tick.h"
#include "common/Update.h"

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

static const std::string NOURISHMENT = "nourishment";
static const std::string MASS = "mass";
static const std::string MAXMASS = "maxmass";
static const std::string MASSRESERVE = "massreserve";
static const std::string RESERVELIMIT = "reservelimit";
static const std::string STATUS = "status";

// This the amount of energy consumed each tick
const double MetabolizingProperty::energyUnit = 0.0001;

// How much energy we get burning one unit of mass
const double MetabolizingProperty::energyToMass = 10.0;

// How much energy creature is able to store as mass (relative to mass) 
// (used if no reserveLimit provided)
const double MetabolizingProperty::defaultReserves = 0.5;

// How much mass creature is able to eat per tick, relative to creature's own mass
const double MetabolizingProperty::biteSize = 0.01;



void MetabolizingProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::TICK_NO, name);
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
 
    debug(std::cout << "MetabolizingProperty::tick_handler(entId="
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

    // atm the metabolizng property value has no interpretation
    double & metabolizing = data(); 
    
    StatusProperty * status_prop = e->requirePropertyClass<StatusProperty>(STATUS, 1.0f);
    bool status_changed = false;
    assert(status_prop != 0);
    status_prop->setFlags(flag_unsent);
    double & status = status_prop->data();

    // get mass reserves (new property introduced for metabolism to work)
    Property<double> * massreserve_prop = e->modPropertyType<double>(MASSRESERVE);

    // get max mass reserves
    const Property<double> * reservelimit_prop = e->getPropertyType<double>(RESERVELIMIT);
    double reserveLimit = defaultReserves;
    if (reservelimit_prop != 0) {
        reserveLimit = reservelimit_prop->data();
    }
  
    // get mass property
    Property<double> * mass_prop = e->modPropertyType<double>(MASS);

    // get nourishment property
    Property<double> * nourishment_prop = e->modPropertyType<double>(NOURISHMENT); 
    // TODO later we should use unified nourishment property instead of nourishment
    
    double nourishmentConsumed = 0;
    
    // DIGEST FIRST 
    // don't go further if we don't have following properties
    if (nourishment_prop != 0 && mass_prop != 0) {

        double & nourishment = nourishment_prop->data();
        double & mass = mass_prop->data();

        // set the nourishment bite size depending on creature size
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

    // Status should be between 0 and ?
    // WEIGHT GAIN for high status
    if (status > (1.5 + energyUnit)) {
        status -= energyUnit;
        status_changed = true;
        if (mass_prop != 0 && massreserve_prop != 0 ) {
            double & mass = mass_prop->data();
            double & massReserve = massreserve_prop->data();
            double massChange = mass*energyUnit/energyToMass;

            Element maxmass_attr;
            if (e->getAttrType(MAXMASS, maxmass_attr, Element::TYPE_FLOAT) == 0) {
                massChange = std::min(massChange, maxmass_attr.Float()-mass);
            }
            // TODO 
            // Change the mass reserves depending on how fat we are
            //
            // we have small reserves put all mass gain into it
            if (massReserve < mass*0.5*reserveLimit) {
              massReserve += reserveLimit * massChange;
            }
            // normal reserves
            else if (massReserve < mass*reserveLimit) {
              massReserve += massChange*reserveLimit * 0.5;
            } 
            else {
              massReserve = massChange*reserveLimit;
            }
            mass += massChange;

            mass_prop->setFlags(flag_unsent);
            massreserve_prop->setFlags(flag_unsent);
            mass_prop->apply(e);
            massreserve_prop->apply(e);
        }
    }
    // increase energy
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


    Update update;              // do i need to do it?
    update->setTo(e->getId());

    res.push_back(update);

    return OPERATION_IGNORED;
}
