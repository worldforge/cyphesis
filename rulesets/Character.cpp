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


#include "Character.h"

#include "Pedestrian.h"
#include "ProxyMind.h"
#include "EntityProperty.h"
#include "ExternalMind.h"
#include "ExternalProperty.h"
#include "OutfitProperty.h"
#include "StatusProperty.h"
#include "TasksProperty.h"
#include "Domain.h"
#include "Task.h"

#include "BaseWorld.h"
#include "common/op_switch.h"
#include "common/const.h"
#include "common/custom.h"
#include "common/debug.h"
#include "common/Link.h"
#include "common/TypeNode.h"
#include "common/PropertyManager.h"

#include "common/Actuate.h"
#include "common/Attack.h"
#include "common/Eat.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"
#include "common/Think.h"
#include "common/Thought.h"
#include "TransientProperty.h"
#include "UsagesProperty.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sigc++/adaptors/hide.h>
#include <common/id.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

// This figure is calculated to allow a character to live for 4 weeks
// without food, during which time they will lose 40% of their mass
// before starving.
const double Character::energyConsumption = 0.0001;

// Food consumption is fast, to keep Acorn playable
const double Character::foodConsumption = 0.1;

// When the character is starving, they start to lose weight. During
// the latter 2 and a half weeks, they lose about 40% of their mass.
// Bad players who do not feed their characters will be punished.
const double Character::weightConsumption = 0.00002;

// Ammount of evergy turned into weight by metabolism when Character
// is well fed.
const double Character::energyLaidDown = 0.1;

// Ammount of weight gained as a result. High for Acorn.
const double Character::weightGain = 0.5;

void Character::destroy()
{
    m_proxyMind = nullptr;

    Entity::destroy();
}

static const std::string FOOD = "food";
static const std::string MASS = "mass";
static const std::string MAXMASS = "maxmass";
static const std::string RIGHT_HAND_WIELD = "right_hand_wield";
static const std::string SERIALNO = "serialno";
static const std::string STAMINA = "stamina";
static const std::string TASKS = "tasks";

/// \brief Calculate how the Characters metabolism has affected it in the
/// last tick
///
/// This function is called every 90 seconds. It does one of three things.
/// If energy is very high, it loses some, and gains some weight. Otherwise
/// it loses some energy, unless energy is very low, in which case loss
/// is slower, as weight is used to compensate.
/// A fully healthy Character should take about a week to starve to death.
/// So 10080 / 90 = 6720 ticks.
/// @param res Any result of changes is returned here.
/// @param ammount Time scale factor, currently not used.
void Character::metabolise(OpVector & res, double ammount)
{
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.

    StatusProperty * status_prop = modPropertyClassFixed<StatusProperty>();
    bool status_changed = false;
    if (status_prop == nullptr) {
        // FIXME Probably don't do enough here to set up the property.
        status_prop = new StatusProperty;
        assert(status_prop != 0);
        m_properties[StatusProperty::property_name] = status_prop;
        status_prop->set(1.f);
        status_changed = true;
    }
    double & status = status_prop->data();
    status_prop->addFlags(flag_unsent);

    Property<double> * food_prop = modPropertyType<double>(FOOD);
    // DIGEST
    if (food_prop != nullptr) {
        double & food = food_prop->data();
        if (food >= foodConsumption && status < 2) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            status += foodConsumption;
            status_changed = true;
            food -= foodConsumption;

            food_prop->addFlags(flag_unsent);
            food_prop->apply(this);
            propertyApplied(FOOD, *food_prop);

        }
    }

    Property<double> * mass_prop = modPropertyType<double>(MASS);
    // If status is very high, we gain weight
    if (status > (1.5 + energyLaidDown)) {
        status -= energyLaidDown;
        status_changed = true;
        if (mass_prop != nullptr) {
            double & mass = mass_prop->data();
            mass += weightGain;
            mass_prop->addFlags(flag_unsent);
            Element maxmass_attr;
            if (getAttrType(MAXMASS, maxmass_attr, Element::TYPE_FLOAT) == 0) {
                mass = std::min(mass, maxmass_attr.Float());
            }
            mass_prop->apply(this);
            propertyApplied(MASS, *mass_prop);
        }
    } else {
        // If status is relatively is not very high, then energy is burned
        double energy_used = energyConsumption * ammount;
        status -= energy_used;
        status_changed = true;
        if (mass_prop != nullptr) {
            double & mass = mass_prop->data();
            double weight_used = weightConsumption * mass * ammount;
            if (status <= 0.5 && mass > weight_used) {
                // Drain away a little less energy and lose some weight
                // This ensures there is a long term penalty to allowing
                // something to starve
                status += (energy_used / 2);
                status_changed = true;
                mass -= weight_used;
                mass_prop->addFlags(flag_unsent);
                mass_prop->apply(this);
                propertyApplied(MASS, *mass_prop);
           }
        }
    }
    // FIXME Stamina property?

    //TODO The idea here seems to be that as long as there's no active tasks, stamina should recover.
    // This needs to be refactored into a different system.
    auto tp = getPropertyClass<TasksProperty>(TASKS);
    if ((tp == nullptr || !tp->busy())) {

        Property<double> * stamina_prop = modPropertyType<double>(STAMINA);
        if (stamina_prop != nullptr) {
            double & stamina = stamina_prop->data();
            if (stamina < 1.f) {
                stamina = 1.f;
                stamina_prop->addFlags(flag_unsent);
                stamina_prop->apply(this);
                propertyApplied(STAMINA, *stamina_prop);
            }
        }
    }

    if (status_changed) {
        status_prop->apply(this);
    }

    Update update;
    update->setTo(getId());

    res.push_back(update);
}

/// \brief Hooked to the Entity::containered signal of the wielded entity
/// to indicate a change of location
///
/// This function responds by removing it as a wielded entity.
void Character::wieldDropped()
{
    Wield wield;
    wield->setTo(getId());
    sendWorld(wield);
}

/// \brief Search for an entity in an entities contents
///
/// Recursive function the finds an entity by ID in another entities
/// contains list.
/// @param ent Entity to search in
/// @param id Identifier of entity to search for
LocatedEntity * Character::findInContains(LocatedEntity * ent,
                                          const std::string & id)
{
    if (ent->m_contains == nullptr) {
        return nullptr;
    }

    if (ent->m_contains != nullptr) {
        for (auto& child : *ent->m_contains) {
            if (child->getId() == id) {
                return child.get();
            }
            if (child->m_contains != nullptr && !child->m_contains->empty()) {
                auto found = findInContains(child.get(), id);
                if (found != nullptr) {
                    return found;
                }
            }
        }
    }
    return nullptr;
}

/// \brief Character constructor
///
/// @param id String identifier
/// @param intId Integer identifier
Character::Character(const std::string & id, long intId) :
           Thing(id, intId),
           m_proxyMind(new ProxyMind(id, intId, *this))
{
}

Character::~Character()
{
    if (m_rightHandWieldConnection.connected()) {
        m_rightHandWieldConnection.disconnect();
    }
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        error(op, "Nourish has no argument.", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    Element mass_attr;
    if (arg->copyAttr(MASS, mass_attr) != 0 || !mass_attr.isNum()) {
        return;
    }

    Property<double> * food_prop = requirePropertyClass<Property<double> >(FOOD, 0.f);
    double & food = food_prop->data();
    food += mass_attr.asNum();
    food_prop->addFlags(flag_unsent);

    // FIXME This will become a Update once private properties are sorted
    Anonymous food_ent;
    food_ent->setId(getId());
    food_ent->setAttr(FOOD, food);

    Set s;
    s->setArgs1(food_ent);
    // FIXME FROM, SECONDS?

    Sight si;
    si->setTo(getId());
    si->setArgs1(s);
    res.push_back(si);
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        std::set<const LocatedEntity*> prevObserving, newObserving;
        EntityProperty * rhw = modPropertyClass<EntityProperty>(RIGHT_HAND_WIELD);
        if (rhw == nullptr) {
            return;
        }

        auto wieldedEntity = rhw->data().get();
        if (wieldedEntity) {
            wieldedEntity->collectObservers(prevObserving);
        }

        rhw->data() = WeakEntityRef(nullptr);
        rhw->addFlags(flag_unsent);
        // FIXME Remove the property?

        // FIXME Make sure we stop wielding if the container changes,
        // but connections are cleared, and don't build up.
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        Update update;
        update->setTo(getId());
        res.push_back(update);

        if (wieldedEntity) {
            wieldedEntity->processAppearDisappear(std::move(prevObserving), res);
        }

        return;
    }
    const Root & arg = op->getArgs().front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Wield arg has no ID", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    auto item = BaseWorld::instance().getEntity(id);
    if (!item) {
        error(op, "Wield arg does not exist", res, getId());
        return;
    }

    if (m_contains == nullptr || m_contains->find(item) == m_contains->end()) {
        error(op, String::compose("Wield arg %1(%2) is not in inventory "
                "of %3(%4)", item->getType()->name(), id, getType()->name(), getId()), res, getId());
        return;
    }

    Element worn_attr;
    if (item->getAttr("worn", worn_attr) == 0) {
        debug(std::cout << "Got wield for a garment" << std::endl << std::flush
        ;);

        if (worn_attr.isString()) {
            std::set<const LocatedEntity*> oldEntityPrevObserving, newEntityPrevObserving;

            OutfitProperty * outfit = requirePropertyClassFixed<OutfitProperty>();
            LocatedEntity* prevEntity = outfit->getEntity(worn_attr.String());
            if (prevEntity) {
                prevEntity->collectObservers(oldEntityPrevObserving);
            }
            item->collectObservers(newEntityPrevObserving);
            outfit->wear(this, worn_attr.String(), item.get());
            outfit->cleanUp();

            outfit->addFlags(flag_unsent);

            if (prevEntity) {
                prevEntity->processAppearDisappear(std::move(oldEntityPrevObserving), res);
            }
            item->processAppearDisappear(std::move(newEntityPrevObserving), res);

        } else {
            log(WARNING, "Got clothing with non-string worn attribute. " + describeEntity());
            return;
        }
        // FIXME Implement adding stuff to the outfit propert, as efficiently
        // as possible
        // Must make sure that we can install the entity we have already
        // looked up here, and fix the GuiseProperty code so it does not
        // need a repeat lookup
    } else {
        std::set<const LocatedEntity*> oldEntityPrevObserving, newEntityPrevObserving;
        debug(std::cout << "Got wield for a tool" << std::endl << std::flush
        ;);

        EntityProperty * rhw = requirePropertyClass<EntityProperty>(RIGHT_HAND_WIELD);
        LocatedEntity* prevEntity = rhw->data().get();
        if (prevEntity) {
            prevEntity->collectObservers(oldEntityPrevObserving);
        }
        item->collectObservers(newEntityPrevObserving);
        // FIXME Make sure we don't stay linked to the previous wielded
        // tool.
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        // The value is ignored by the update handler, but should be the
        // right type.
        rhw->data() = WeakEntityRef(item);
        rhw->addFlags(flag_unsent);

        m_rightHandWieldConnection = item->containered.connect(sigc::hide<0>(sigc::mem_fun(this, &Character::wieldDropped)));

        if (prevEntity) {
            prevEntity->processAppearDisappear(std::move(oldEntityPrevObserving), res);
        }
        item->processAppearDisappear(std::move(newEntityPrevObserving), res);

        debug(std::cout << "Wielding " << item->getId() << std::endl << std::flush
        ;);
    }

    Update update;
    update->setTo(getId());
    res.push_back(update);
}
