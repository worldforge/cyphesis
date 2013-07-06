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

#include "common/BaseWorld.h"
#include "common/compose.hpp"
#include "common/Link.h"
#include "common/log.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Imaginary;

static const double character_expire_time = 60 * 60; // 1 hour

void ExternalMind::deleteEntity(const std::string & id)
{
    Delete d;
    Anonymous del_arg;
    del_arg->setId(id);
    d->setArgs1(del_arg);
    d->setTo(id);
    m_entity.sendWorld(d);
}

void ExternalMind::purgeEntity(const LocatedEntity & ent)
{
    if (ent.m_contains != 0) {
        LocatedEntitySet::const_iterator I = ent.m_contains->begin();
        LocatedEntitySet::const_iterator Iend = ent.m_contains->end();
        for (; I != Iend; ++I) {
            LocatedEntity * child = *I;
            assert(child != 0);
            purgeEntity(*child);
        }
    }
    deleteEntity(ent.getId());
}

ExternalMind::ExternalMind(LocatedEntity & e) : Router(e.getId(), e.getIntId()),
                                                m_external(0),
                                                m_entity(e),
                                                m_lossTime(0.)
{
}

ExternalMind::~ExternalMind()
{
}

void ExternalMind::externalOperation(const Operation & op, Link &)
{
    log(ERROR, String::compose("%1 called", __PRETTY_FUNCTION__));
}

void ExternalMind::operation(const Operation & op, OpVector & res)
{
    if (m_external == 0) {
        if (m_entity.getFlags() & entity_ephem) {
            // If this entity no longer has a connection, and is ephemeral
            // we should delete it.
            if (op->getClassNo() != Atlas::Objects::Operation::DELETE_NO) {
                purgeEntity(m_entity);
            }
        }
        // std::cout << "Time since disco "
                  // << BaseWorld::instance().getTime() - m_lossTime
                  // << std::endl << std::flush;
        if (BaseWorld::instance().getTime() - m_lossTime > character_expire_time) {
            if (op->getClassNo() != Atlas::Objects::Operation::DELETE_NO) {
                purgeEntity(m_entity);
            }
        }
        return;
    }
    m_external->send(op);

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

                    m_external->send(sight);
                }
            }
        }
    }
}

const std::string & ExternalMind::connectionId()
{
    assert(m_external != 0);
    return m_external->getId();
}

void ExternalMind::linkUp(Link * c)
{
    m_external = c;
    if (c == 0) {
        m_lossTime = BaseWorld::instance().getTime();
    }
}
