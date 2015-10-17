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
using Atlas::Message::Element;

static const bool debug_flag = true;

static const std::string FOOD = "food";
static const std::string MASS = "mass";
static const std::string MAXMASS = "maxmass";
static const std::string STATUS = "status";

// This the amount of energy consumed each tick
const double MetabolizingProperty::energyUnit = 0.0001;

// How much energy we get burning one unit of mass
const double MetabolizingProperty::energyToMass = 10.0;

// How much energy creature is able to store as mass (relative to mass) 
const double MetabolizingProperty::reserves = 0.5;

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

HandlerResult MetabolizingProperty::tick_handler(LocatedEntity * e,
                                           const Operation & op,
                                           OpVector & res)
{

    // The value of the property has interpretation of mass reserve
    // that can be transformed into energy and sustain the entity
    // that's why the name is changed
    double & massReserve = data(); 
    double massReserveChange = 0;

    StatusProperty * status_prop = e->requirePropertyClass<StatusProperty>(STATUS);
    bool status_changed = false;
    assert(status_prop != 0);
    status_prop->setFlags(flag_unsent);
    double & status = status_prop->data();

    // get mass property
    Property<double> * mass_prop = e->modPropertyType<double>(MASS);

    // get food property
    Property<double> * food_prop = e->modPropertyType<double>(FOOD); 
    // TODO later we should use unified nourishment property instead of food
    
    double foodConsumed = 0;
    
    // DIGEST FIRST 
    // don't go further if we don't have following properties
    if (food_prop != 0 && mass_prop != 0) {

        double & food = food_prop->data();
        double & mass = mass_prop->data();

        // set the food bite size depending on creature size
        foodConsumed = std::min(food, mass*biteSize);
        
        food -= foodConsumed;

        food_prop->setFlags(flag_unsent);
        food_prop->apply(e);

        if (foodConsumed > 0) {
          status += biteSize*energyToMass;
          status_changed = true;
        }
    }

    // Status should be between 0 and ?
    // WEIGHT GAIN for high status
    if (status > (1.5 + energyUnit)) {
        status -= energyUnit;
        status_changed = true;
        if (mass_prop != 0) {
            double & mass = mass_prop->data();
            double massChange = mass*energyUnit/energyToMass;
            mass_prop->setFlags(flag_unsent);
            Element maxmass_attr;
            if (e->getAttrType(MAXMASS, maxmass_attr, Element::TYPE_FLOAT) == 0) {
                massChange = std::min(massChange, maxmass_attr.Float()-mass);
            }
            // TODO 
            // change the portion of massReserve gain depending on current reserves
            if (massReserve < mass*reserves) {
              massReserveChange = massChange;
            }
            else {
              massReserveChange = massChange*reserves;
            }
            mass += massChange;
            mass_prop->apply(e);
        }
    }
    // increase energy
    else {
        status -= energyUnit;
        status_changed = true;
        if (mass_prop != 0) {
            double & mass = mass_prop->data();
            double massBurn = mass*energyUnit/energyToMass;
            if (status <= 0.5 && massReserve > massBurn) {
                // Drain away a little less energy and lose some weight
                // This ensures there is a long term penalty to allowing
                // something to starve
                status += energyUnit;
                status_changed = true;
                mass -= massBurn;
                massReserveChange = -massBurn;
                mass_prop->setFlags(flag_unsent);
                mass_prop->apply(e);
            }
        }
    }

    if (status_changed) {
        status_prop->apply(e);
    }

    // How to set the value of the property form within itself ?
    // Should I send send operation instead? Is it the proper way to do it?
    this->setFlags(flag_unsent);
    this->apply(e);

    Update update;              // do i need to do it?
    update->setTo(e->getId());

    res.push_back(update);

    return OPERATION_IGNORED;
}
