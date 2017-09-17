// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#include "World.h"

#include "CalendarProperty.h"
#include "AtlasProperties.h"
#include "Domain.h"

#include "common/BaseWorld.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/custom.h"

#include "common/Eat.h"
#include "common/Nourish.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using Atlas::Objects::smart_dynamic_cast;

typedef enum { ROCK = 0, SAND = 1, GRASS = 2, SILT = 3, SNOW = 4} Surface;

/// \brief Constructor for the World entity
World::World(const std::string & id, long intId) : Thing(id, intId), m_serialNumber(0)
{

    CalendarProperty* calProp = new CalendarProperty();
    calProp->install(this, "calendar");
    m_properties["calendar"] = calProp;
}

World::~World()
{
}

void World::LookOperation(const Operation & op, OpVector & res)
{
    // We must be the top level entity
    assert(m_location.m_loc == 0);
    // We must contains something, or where the hell did the look come from?
    assert(m_contains != 0);

    //The top level entity is a little special, since its properties can be inspected by all entities, although it's children can not.
    //First check if there's a movement domain. If so we'll handle Look ops just like usually. However, if not we'll send the properties sans the "contains" property.
    LocatedEntity * from = BaseWorld::instance().getEntity(op->getFrom());
    if (from == nullptr) {
        log(ERROR, String::compose("Look op has invalid from %1. %2", op->getFrom(), describeEntity()));
        return;
    }

    // Register the entity with the world router as perceptive.
    BaseWorld::instance().addPerceptive(from);

    Domain* domain = nullptr;
    if (m_location.m_loc) {
        domain = m_location.m_loc->getDomain();
    }
    if (domain) {
        generateSightOp(*from, op, res);
    } else {
        Sight s;

        Anonymous sarg;
        addToEntity(sarg);
        //Hide all contents of the root entity.
        sarg->removeAttr("contains");
        s->setArgs1(sarg);
        s->setTo(op->getFrom());
        res.push_back(s);
    }
}

void World::MoveOperation(const Operation & op, OpVector & res)
{
    assert(m_location.m_loc == 0);
    // Can't move the world.
}

void World::DeleteOperation(const Operation & op, OpVector & res)
{
    //A delete operation with an argument sent to the world indicates that an
    //entity should be deleted forcefully (whereas a Delete operation sent to
    //an entity directly, which is the norm, always can be overridden by the entity).
    auto& args = op->getArgs();
    if (!args.empty()) {
        auto arg = args.front();
        if (!arg->isDefaultId()) {
            auto entity = BaseWorld::instance().getEntity(arg->getId());
            if (entity == this) {
                Atlas::Message::Element force;
                if (arg->copyAttr("force", force) == 0 && force.isInt() && force.asInt() == 1) {
                    clearWorld(res);
                } else {
                    log(ERROR, "World::DeleteOperation cannot delete world unless 'force' flag is set.");
                }
            } else {
                BaseWorld::instance().delEntity(entity);
            }
        } else {
            log(ERROR, "World::DeleteOperation got delete op with arg but no id.");
        }
    } else {
        assert(m_location.m_loc == 0);
        // Deleting has no effect.
    }
}

void World::clearWorld(OpVector & res) {
    log(INFO, "Clearing world; deleting all entities.");

    OpVector ignoredRes;
    auto& baseWorld = BaseWorld::instance();
    if (m_contains) {
        while (!m_contains->empty()) {

            LocatedEntity* entity = *m_contains->begin();

            if (entity->isPerceptive()) {
                //Send a sight of a delete op to the entity so that it knows it has been deleted.
                Delete delOp;
                delOp->setTo(entity->getId());

                Anonymous delArg;
                delArg->setId(entity->getId());
                delOp->setArgs1(delArg);

                Sight sToEntity;
                sToEntity->setArgs1(delOp);
                sToEntity->setTo(entity->getId());
                entity->operation(sToEntity, ignoredRes);
            }
            baseWorld.delEntity(entity);
        }
    }

    //Remove all properties except for "id"
    auto propIter = m_properties.begin();
    while(propIter != m_properties.end())
    {
        if (propIter->first != "id") {
            auto prop = propIter->second;
            prop->remove(this, propIter->first);
            delete prop;
            m_properties.erase(propIter++);
        } else {
            ++propIter;
        }
    }

    CalendarProperty* calProp = new CalendarProperty();
    calProp->install(this, "calendar");
    m_properties["calendar"] = calProp;

    delete m_contains;
    m_contains = nullptr;

    log(INFO, "World cleared of all entities.");
}

void World::RelayOperation(const Operation & op, OpVector & res)
{
    //A Relay operation with refno sent to ourselves signals that we should prune
    //our registered relays in m_relays. This is a feature to allow for a timeout; if
    //no Relay has been received from the destination Entity after a certain period
    //we'll shut down the relay link.
    if (op->getTo() == getId() && op->getFrom() == getId() && !op->isDefaultRefno()) {
        auto I = m_relays.find(op->getRefno());
        if (I != m_relays.end()) {

            //Send an empty operation to signal that the relay has expired.
            I->second.callback(Operation(), I->second.entityId);
            m_relays.erase(I);
        }
    } else {
        if (op->getArgs().empty()) {
            log(ERROR, "World::RelayOperation no args.");
            return;
        }
        Operation relayedOp = Atlas::Objects::smart_dynamic_cast<Operation>(
                op->getArgs().front());

        if (!relayedOp.isValid()) {
            log(ERROR,
                    "World::RelayOperation first arg is not an operation.");
            return;
        }


        //If a relay op has a refno, it's a response to a Relay op previously sent out to another
        //entity, and we should signal that we have an incoming relayed op.
        if (!op->isDefaultRefno()) {
            //Note that the relayed op should be considered untrusted in this case, as it has originated
            //from a random entity or its mind.
            auto I = m_relays.find(op->getRefno());
            if (I == m_relays.end()) {
                log(WARNING,
                        "World::RelayOperation could not find registrered Relay with refno.");
                return;
            }

            //Make sure that this op really comes from the entity the original Relay op was sent to.
            if (op->getFrom() != I->second.entityId) {
                log(WARNING,
                        "World::RelayOperation got relay op with mismatching 'from'.");
                return;
            }

            //Get the relayed operation and call the callback.
            I->second.callback(relayedOp, I->second.entityId);

            m_relays.erase(I);

        } else {
            //Send it on to the basic Entity relay mechanism
            Entity::RelayOperation(op, res);
        }
    }

}

void World::sendRelayToEntity(const LocatedEntity& to, const Operation& op, sigc::slot<void, const Operation&, const std::string&> callback)
{
    //Make the op appear to come from the destination entity.
    op->setFrom(to.getId());

    long int serialNo = ++m_serialNumber;
    Atlas::Objects::Operation::Generic relayOp;
    relayOp->setType("relay", Atlas::Objects::Operation::RELAY_NO);
    relayOp->setTo(to.getId());
    relayOp->setSerialno(serialNo);
    relayOp->setArgs1(op);
    Relay relay;
    relay.entityId = to.getId();
    relay.callback = callback;
    m_relays.insert(std::make_pair(serialNo, relay));

    sendWorld(relayOp);

    //Also send a future Relay op to ourselves to make sure that the registered relay in m_relays
    //is removed in the case that we don't get any response.
    Atlas::Objects::Operation::Generic pruneOp;
    pruneOp->setType("relay", Atlas::Objects::Operation::RELAY_NO);
    pruneOp->setTo(getId());
    pruneOp->setFrom(getId());
    pruneOp->setRefno(serialNo);
    pruneOp->setFutureSeconds(5);
    sendWorld(pruneOp);
}

