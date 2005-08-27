// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// A stackable object, ie one which can represent multiple object of the
// same type. Used for things like coins.

#include "Stackable.h"

#include "Script.h"

#include "common/Property.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Entity::Anonymous;

Stackable::Stackable(const std::string & id) : Stackable_parent(id),
                                               m_num(1)
{
    subscribe("combine", OP_COMBINE);
    subscribe("divide", OP_DIVIDE);

    m_properties["num"] = new Property<int>(m_num, 0);
}

Stackable::~Stackable()
{
}

void Stackable::CombineOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("combine", op, res) != 0) {
        return;
    }
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
        Entity * ent = m_world->getEntity(id);
        if (ent == NULL) {
            // FIXME Send an Unseen op?
            continue;
        }
        Stackable * obj = dynamic_cast<Stackable *>(ent);
        if (obj == NULL) { continue; }
        if (obj->m_type != m_type) { continue; }
        m_num = m_num + obj->m_num;
        // Set op for num change?

        Delete d;
        Anonymous del_arg;
        del_arg->setId(id);
        d->setTo(id);
        d->setArgs1(del_arg);
        res.push_back(d);
    }
    // Currently does not send sight ops, as the Sight ops for this type of
    // thing have not been discussed
}

void Stackable::DivideOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("divide", op, res) != 0) {
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    std::vector<Root>::const_iterator Iend = args.end();
    for (std::vector<Root>::const_iterator I = args.begin(); I != Iend; ++I) {
        const Root & arg = *I;
        int new_num = 1;
        Element num_attr;
        if (arg->getAttr("num", num_attr) != 0 && num_attr.isInt()) {
            new_num = num_attr.asInt();
        }
        if (m_num <= new_num) { continue; }
        
        Anonymous create_arg;
        create_arg->setParents(std::list<std::string>(1, m_type));
        create_arg->setAttr("num", new_num);
        Create c;
        c->setArgs1(create_arg);
        c->setTo(getId());
        res.push_back(c);
    }
    // Currently does not send sight ops, as the Sight ops for this type of
    // thing have not been discussed
}
