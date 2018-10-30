// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include "Flusher.h"

#include "tools/ObjectContext.h"

#include "common/compose.hpp"
#include "common/operations/Tick.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Tick;

using std::shared_ptr;

Flusher::Flusher(const shared_ptr<ObjectContext> & c) : m_context(c)
{
}

Flusher::~Flusher() = default;

void Flusher::setup(const std::string & arg, OpVector & ret)
{
    shared_ptr<ObjectContext> flush_context = m_context.lock();

    if (!flush_context) {
        m_complete = true;
        return;
    }

    type = arg;

    m_description = String::compose("flushing %1", type);

    // Send a look to search by type.
    Look l;

    Anonymous lmap;
    lmap->setParent(type);
    l->setArgs1(lmap);

    flush_context->setFromContext(l);

    ret.push_back(l);
}

void Flusher::operation(const Operation & op, OpVector & res)
{
    shared_ptr<ObjectContext> flush_context = m_context.lock();

    if (!flush_context) {
        m_complete = true;
        return;
    }

    if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        // We have a sight op, check if its the sight of an entity we
        // want to delete.
        const std::vector<Root> & args = op->getArgs();
        if (args.empty()) {
            std::cerr << "Got empty sight" << std::endl << std::flush;
            return;
        }
        const Root & arg = args.front();
        assert(arg.isValid());
        RootEntity sight_ent = smart_dynamic_cast<RootEntity>(arg);
        if (!sight_ent.isValid()) {
            return;
        }
        if (!sight_ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            std::cerr << "Got sight no ID" << std::endl << std::flush;
            return;
        }
        if (!sight_ent->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
            std::cerr << "Got sight no PARENT" << std::endl << std::flush;
            return;
        }
        if (sight_ent->getParent() != type) {
            return;
        }
        const std::string & id = sight_ent->getId();

        std::cout << "Deleting: " << type << "(" << id << ")"
                  << std::endl << std::flush;

        // Send a delete to the entity we have seen.
        Delete d;

        Anonymous dmap;
        dmap->setId(id);
        d->setArgs1(dmap);

        flush_context->setFromContext(d);

        d->setTo(id);

        res.push_back(d);

        // Send a tick for a short time in the future so that
        // we can look again once this entity is definitly gone.
        Tick t;

        Anonymous tick_arg;
        tick_arg->setName("flusher");

        flush_context->setFromContext(t);
        t->setTo(t->getFrom());
        t->setFutureSeconds(0.1);
        t->setArgs1(tick_arg);

        res.push_back(t);
    } else if (op->getParent() == "tick") {
        // We have a tick op, check if its the one we sent ourselves
        // to schedule the next look.
        if (op->getArgs().empty() ||
            op->getArgs().front()->getName() != "flusher") {
            std::cout << "Not for us" << std::endl << std::flush;
            return;
        }

        // Send another look by type.
        Look l;

        Anonymous lmap;
        lmap->setParent(type);
        l->setArgs1(lmap);
        flush_context->setFromContext(l);

        res.push_back(l);
    } else if (op->getParent() == "unseen") {
        // We have an unseen op, which signals our last look returned
        // no results.
        m_complete = true;
    }
}
