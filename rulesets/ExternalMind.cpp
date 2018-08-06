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

#include "rulesets/LocatedEntity.h"

#include "BaseWorld.h"
#include "common/Link.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <common/Think.h>
#include <common/Thought.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Imaginary;

static const double character_expire_time = 60 * 60; // 1 hour

void ExternalMind::deleteEntity(const std::string & id, bool forceDelete)
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
    m_entity.sendWorld(d);
}

void ExternalMind::purgeEntity(const LocatedEntity & ent, bool forceDelete)
{
    if (ent.m_contains != nullptr) {
        for (auto& child : *ent.m_contains) {
            assert(child != nullptr);
            purgeEntity(*child);
        }
    }
    deleteEntity(ent.getId(), forceDelete);
}

ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                                m_link(nullptr),
                                                m_entity(e),
                                                m_lossTime(0.)
{
}

void ExternalMind::addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const
{
    ent->setObjtype("obj");
    ent->setId(getId());

    Anonymous entityAttr;
    entityAttr->setId(m_entity.getId());
    ent->setAttr("entity", entityAttr->asMessage());
}


void ExternalMind::externalOperation(const Operation & op, Link & link)
{
    //The entity only allows external Thought ops, so we need to wrap the ones we receive from the client.
    Atlas::Objects::Operation::Thought thought{};
    thought->setTo(m_entity.getId());
    thought->setArgs1(op);

    OpVector res;
    m_entity.operation(thought, res);

    for (auto& resOp : res) {
        m_entity.sendWorld(resOp);
    }

}

void ExternalMind::operation(const Operation & op, OpVector & res)
{

    //TODO: remove this hacked in fix.
    //So, what's happening here is that if the external connection has been disconnected, and
    //the entity either is marked as ephemeral, or has been inactive for an hour, it is
    //deleted along with all of its inventory. This is to prevent the starting position
    //being spammed by abandoned characters. Now, there are a lot of better ways to handle this.
    //One solution is to make sure that all new entities are created in an instanced location.
    //Thus players have to actively move from the instanced location to the main world, which
    //would make sure that only those players which are active end up in the real world.
    //Another solution is to do something with the entity when the connection is cut; perhaps move
    //it to limbo or some other place. All of these solutions are better than just deleting it.
    if (m_link == nullptr) {
        if (m_entity.hasFlags(entity_ephem)) {
            // If this entity no longer has a connection, and is ephemeral
            // we should delete it.
            if (op->getClassNo() != Atlas::Objects::Operation::DELETE_NO) {
                purgeEntity(m_entity);
            }
        }
//        if (BaseWorld::instance().getTime() - m_lossTime > character_expire_time) {
//            if (op->getClassNo() != Atlas::Objects::Operation::DELETE_NO) {
//                //reset m_lossTime since it's not a given that the entity will be deleted
//                //(properties such as respawnable might intervene)
//                m_lossTime = BaseWorld::instance().getTime();
//                purgeEntity(m_entity);
//            }
//        }
        return;
    }
    m_link->send(op);

    // Here we see if there is anything we should be sending the user
    // extra info about. The initial demo implementation checks for
    // Set ops which make the characters status less than 0.1, and sends
    // emotes that the character is hungry.
    const std::vector<Root> & args = op->getArgs();
    if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO && !args.empty()) {
        Operation sub_op = smart_dynamic_cast<Operation>(args.front());
        if (sub_op.isValid()) {
            const std::vector<Root> & sub_args = sub_op->getArgs();
            if (sub_op->getClassNo() == Atlas::Objects::Operation::SET_NO && !sub_args.empty()) {
                const Root & arg = sub_args.front();
                Element status_value;
                if (arg->getId() == getId() and
                    arg->copyAttr("status", status_value) == 0 and
                    status_value.isFloat() and status_value.Float() < 0.1) {

                    Anonymous imaginary_arg;
                    imaginary_arg->setId(getId());
                    if (status_value.Float() < 0.01) {
                        imaginary_arg->setAttr("description", "is starving.");
                    } else {
                        imaginary_arg->setAttr("description", "is hungry.");
                    }

                    Imaginary imaginary;
                    imaginary->setTo(getId());
                    imaginary->setFrom(getId());
                    imaginary->setArgs1(imaginary_arg);

                    Sight sight;
                    sight->setTo(getId());
                    sight->setFrom(getId());
                    sight->setArgs1(imaginary);

                    m_link->send(sight);
                }
            }
        }
    }
}

const std::string & ExternalMind::connectionId()
{
    assert(m_link != nullptr);
    return m_link->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_link = c;
    if (c == nullptr) {
        m_lossTime = BaseWorld::instance().getTime();
    }
}
