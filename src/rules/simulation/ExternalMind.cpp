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


#include "ExternalMind.h"

#include "rules/LocatedEntity.h"

#include "rules/simulation/BaseWorld.h"
#include "common/Link.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <modules/Variant.h>


#include "common/operations/Think.h"
#include "common/operations/Thought.h"
#include "common/operations/Relay.h"

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Imaginary;

long ExternalMind::s_serialNumberNext = 0L;
int ExternalMind::s_numberOfMinds = 0L;


ExternalMind::~ExternalMind()
{
    s_numberOfMinds--;
}

void ExternalMind::deleteEntity(const std::string& id, bool forceDelete)
{
    Delete d;
    Anonymous del_arg;
    del_arg->setId(id);
    d->setArgs1(del_arg);
    if (forceDelete) {
        //We can force a delete by directing the Delete op to the world;
        //this bypasses any property which might prevent the deletion.
        d->setTo("0");
    } else {
        d->setTo(id);
    }
    m_entity->sendWorld(d);
}

void ExternalMind::purgeEntity(const LocatedEntity& ent, bool forceDelete)
{
    if (ent.m_contains != nullptr) {
        for (auto& child : *ent.m_contains) {
            assert(child != nullptr);
            purgeEntity(*child);
        }
    }
    deleteEntity(ent.getId(), forceDelete);
}

ExternalMind::ExternalMind(RouterId id, Ref<LocatedEntity> entity)
        : Router(std::move(id)),
          m_link(nullptr),
          m_entity(std::move(entity))
{
    s_numberOfMinds++;
}

const Ref<LocatedEntity>& ExternalMind::getEntity() const
{
    return m_entity;
}

void ExternalMind::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ent->setObjtype("obj");
    ent->setId(getId());

    Anonymous entityAttr;
    entityAttr->setId(m_entity->getId());
    ent->setAttr("entity", entityAttr->asMessage());
}


void ExternalMind::externalOperation(const Operation& op, Link& link)
{
    //Any operations coming from the mind with a refno is a response to a previously Relayed op, and need to be handled.
    if (!op->isDefaultRefno()) {
        externalRelayedOperation(op);
    } else {
        if (op->getClassNo() == Atlas::Objects::Operation::GET_NO) {
            OpVector res;
            GetOperation(op, res);
            for (auto& resOp : res) {
                resOp->setTo(getId());
                link.send(resOp);
            }
        } else {
            OpVector res;

            //Any ops coming from the mind must be Thought ops.
            Atlas::Objects::Operation::Thought thought{};
            thought->setTo(m_entity->getId());
            thought->setArgs1(op);

            m_entity->operation(thought, res);

            for (auto& resOp : res) {
                m_entity->sendWorld(resOp);
            }
        }
    }
}

void ExternalMind::operation(const Operation& op, OpVector& res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::RELAY_NO) {
        RelayOperation(op, res);
    } else {
        //Only sent ops that inherit from "Info" to the client.
        if (op->instanceOf(Atlas::Objects::Operation::INFO_NO)) {
            op->setSeconds(BaseWorld::instance().getTimeAsSeconds());
            m_link->send(op);
        }
    }
}

void ExternalMind::RelayOperation(const Operation& op, OpVector& res)
{
    //A Relay operation with refno sent to ourselves signals that we should prune
    //our registered relays in m_relays. This is a feature to allow for a timeout; if
    //no Relay has been received from the destination Entity after a certain period
    //we'll shut down the relay link.
    if (op->getTo() == m_entity->getId() && op->getFrom() == m_entity->getId() && !op->isDefaultRefno()) {
        auto I = m_relays.find(op->getRefno());
        if (I != m_relays.end()) {
            auto& relay = I->second;
            if (!relay.op->isDefaultSerialno() || !relay.op->isDefaultFrom()) {
                //Also send a no-op to any entity to make it stop waiting for any response.
                Atlas::Objects::Operation::Relay noop;
                if (!relay.op->isDefaultSerialno()) {
                    noop->setRefno(relay.op->getSerialno());
                }
                if (!relay.op->isDefaultFrom()) {
                    noop->setTo(relay.op->getFrom());
                }
                noop->setFrom(m_entity->getId());
                noop->setId(relay.from_id);
                m_entity->sendWorld(noop);
            }
            m_relays.erase(I);
        }
    } else {


        //If a relay op has a refno, it's a response to a Relay op previously sent out to another
        //entity, and we should send the incoming relayed operation to the mind.
        if (!op->isDefaultRefno()) {
            //Send the relay op on to the mind
            m_link->send(op);

        } else {


            //If the Relay op instead has a serial no, it's a Relay op sent from us by another Entity
            //which expects a response. We should send it on to the mind (after registering an entry in
            //m_relays to be handled by mind2body).
            //Note that the relayed operation in this case should be considered "trusted", as it has originated
            //from either the server itself or a trusted client.

            //Extract the contained operation, and register the relay into m_relays
            if (op->isDefaultSerialno()) {
                log(ERROR, "ExternalMind::RelayOperation no serial number. " + m_entity->describeEntity());
                return;
            }

            Element from_id;
            if (op->copyAttr("from_id", from_id) != 0 || !from_id.isString()) {
                log(ERROR, "ExternalMind::RelayOperation no valid 'from_id' attribute. " + m_entity->describeEntity());
                return;
            }


            if (op->getArgs().empty()) {
                log(ERROR, "ExternalMind::RelayOperation relay op has no args. " + m_entity->describeEntity());
                return;
            }

            Operation relayedOp = Atlas::Objects::smart_dynamic_cast<Operation>(op->getArgs().front());

            if (!relayedOp.isValid()) {
                log(ERROR, "ExternalMind::RelayOperation first arg is not an operation. " + m_entity->describeEntity());
                return;
            }


            Relay relay{op, from_id.String()};
            //Generate a local serial number which we'll register in m_relays. When a response is received
            //we'll check the refno and match it against what we've stored
            long int serialNo = ++s_serialNumberNext;
            relayedOp->setSerialno(serialNo);
            m_relays.emplace(serialNo, relay);

            m_link->send(relayedOp);

            //Also send a future Relay op to ourselves to make sure that the registered relay in m_relays
            //is removed in the case that we don't get any response.
            Atlas::Objects::Operation::Relay pruneOp;
            pruneOp->setTo(m_entity->getId());
            pruneOp->setFrom(m_entity->getId());
            pruneOp->setRefno(serialNo);
            //5 seconds should be more than enough.
            pruneOp->setFutureSeconds(5);
            //Set id to direct it to this mind
            pruneOp->setId(getId());

            res.push_back(pruneOp);
        }
    }
}

void ExternalMind::externalRelayedOperation(const Operation& op)
{
    //We received an op with a refno from the mind, it's a response to a previously relayed op.
    auto I = m_relays.find(op->getRefno());
    if (I != m_relays.end()) {
        auto& relay = I->second;

        auto& origOp = relay.op;
        if (!origOp->isDefaultFrom() && !origOp->isDefaultSerialno() && !origOp->isDefaultId()) {
            Atlas::Objects::Operation::Relay relayOp;
            relayOp->setTo(origOp->getFrom()); //Send back to the originating entity.
            relayOp->setRefno(origOp->getSerialno()); //Set refno to match serial no.
            relayOp->setId(relay.from_id);
            relayOp->setArgs1(op);
            m_entity->sendWorld(relayOp);
        }
        m_relays.erase(I);
    }

}

const std::string& ExternalMind::connectionId()
{
    assert(m_link != nullptr);
    return m_link->getId();
}

void ExternalMind::linkUp(Link* c)
{
    m_link = c;
}

void ExternalMind::GetOperation(const Operation& op, OpVector& res)
{
    std::vector<Atlas::Objects::Root> rules;
    for (auto& arg: op->getArgs()) {
        if (!arg->isDefaultId()) {
            auto id = arg->getId();
            auto visibility = Visibility::PUBLIC;
            //Check if the id is of the same type as the entity; that means we're allowed to see protected fields.
            if (m_entity->getType()->isTypeOf(id)) {
                visibility = Visibility::PROTECTED;
            }

            auto& o = Inheritance::instance().getClass(id, visibility);
            if (!o.isValid()) {
                clientError(op, String::compose("Unknown type definition for \"%1\" "
                                                "requested", id), res);
                continue;
            }
            rules.emplace_back(o);
        }
    }
    Atlas::Objects::Operation::Info info;
    info->setArgs(std::move(rules));
    if (!op->isDefaultSerialno()) {
        info->setRefno(op->getSerialno());
    }

    res.push_back(info);

}
