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

// $Id: Stackable.cpp,v 1.51 2007-01-01 17:57:09 alriddoch Exp $

// A stackable object, ie one which can represent multiple object of the
// same type. Used for things like coins.

#include "Stackable.h"

#include "Script.h"

#include "common/Property.h"
#include "common/log.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::Anonymous;

/// \brief Stackable constructor
///
/// @param id identifier of this Entity
/// @param intId identifier of this Entity
Stackable::Stackable(const std::string & id, long intId) :
                                             Stackable_parent(id, intId),
                                             m_num(1)
{
    m_properties["num"] = new Property<int>(m_num, 0);
}

Stackable::~Stackable()
{
}

void Stackable::CombineOperation(const Operation & op, OpVector & res)
{
    std::cout << "CombineOperation" << std::endl << std::flush;
    int old_num = m_num;
    const std::vector<Root> & args = op->getArgs();
    std::vector<Root>::const_iterator Iend = args.end();
    for (std::vector<Root>::const_iterator I = args.begin(); I != Iend; ++I) {
        const Root & arg = *I;
        if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            error(op, "Combine op arg has no ID", res, getId());
            continue;
        }
        const std::string & id = arg->getId();
        if (arg->getId() == getId()) {
            // This is normal
            continue;
        }
        Entity * ent = BaseWorld::instance().getEntity(id);
        if (ent == NULL) {
            // FIXME Send an Unseen op?
            continue;
        }
        Stackable * obj = dynamic_cast<Stackable *>(ent);
        if (obj == NULL) { continue; }
        if (obj->m_type != m_type) { continue; }
        m_num = m_num + obj->m_num;
        // This ensures an attempt to stack this entitie multiple times will
        // not result in a dupped item
        obj->m_num = 0;

        Delete d;
        Anonymous del_arg;
        del_arg->setId(id);
        d->setTo(id);
        d->setArgs1(del_arg);
        res.push_back(d);
    }

    if (old_num == m_num) {
        return;
    }

    Set set;
    Anonymous set_arg;
    set_arg->setId(getId());
    set_arg->setAttr("num", m_num);
    set->setArgs1(set_arg);
    set->setTo(getId());

    Sight sight;
    sight->setArgs1(set_arg);
    res.push_back(sight);
}

void Stackable::DivideOperation(const Operation & op, OpVector & res)
{
    int old_num = m_num;
    const std::vector<Root> & args = op->getArgs();
    std::vector<Root>::const_iterator Iend = args.end();
    for (std::vector<Root>::const_iterator I = args.begin(); I != Iend; ++I) {
        const Root & arg = *I;
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            if (arg->getId() != getId()) {
                log(ERROR, "Divide operation has wrong ID in args");
            }
            continue;
        }
        int new_num = 1;
        Element num_attr;
        if (arg->copyAttr("num", num_attr) != 0 && num_attr.isInt()) {
            new_num = num_attr.asInt();
        }
        if (m_num <= new_num) {
            log(ERROR, "Attempt to divide entity into a chunk larger than the original");
            continue;
        }

        m_num -= new_num;
        
        Anonymous create_arg;
        create_arg->setParents(std::list<std::string>(1, m_type));
        if (new_num > 1) {
            create_arg->setAttr("num", new_num);
        }

        Create c;
        c->setArgs1(create_arg);
        c->setTo(getId());
        res.push_back(c);
    }
    // Currently does not send sight ops, as the Sight ops for this type of
    // thing have not been discussed

    if (old_num == m_num) {
        return;
    }

    Set set;
    Anonymous set_arg;
    set_arg->setId(getId());
    set_arg->setAttr("num", m_num);
    set->setArgs1(set_arg);
    set->setTo(getId());

    Sight sight;
    sight->setArgs1(set_arg);
    res.push_back(sight);
}
