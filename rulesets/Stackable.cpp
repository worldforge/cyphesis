// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

// A stackable object, ie one which can represent multiple object of the
// same type. Used for things like coins.

#include "Stackable.h"

#include "Script.h"

#include "common/Property.h"

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;

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
    const ListType & args = op.getArgs();
    ListType::const_iterator Iend = args.end();
    for (ListType::const_iterator I = args.begin(); I != Iend; ++I) {
        const std::string & id = I->asMap().find("id")->second.asString();
        if (id == getId()) { continue; }
        Entity * ent = m_world->getObject(id);
        if (ent == NULL) { continue; }
        Stackable * obj = dynamic_cast<Stackable *>(ent);
        if (obj == NULL) { continue; }
        if (obj->m_type != m_type) { continue; }
        m_num = m_num + obj->m_num;

        Delete * d = new Delete();
        MapType dent;
        dent["id"] = id;
        d->setTo(id);
        d->setArgs(ListType(1,dent));
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
    const ListType & args = op.getArgs();
    ListType::const_iterator Iend = args.end();
    for (ListType::const_iterator I = args.begin(); I != Iend; ++I) {
        const MapType & ent = I->asMap();
        int new_num = 1;
        MapType::const_iterator J = ent.find("num");
        if (J != ent.end()) {
            if (J->second.isInt()) { new_num = J->second.asInt(); }
        }
        if (m_num <= new_num) { continue; }
        
        MapType new_ent;
        ListType parents(1,m_type);
        new_ent["parents"] = parents;
        new_ent["num"] = new_num;
        Create * c = new Create( );
        c->setArgs(ListType(1,new_ent));
        c->setTo(getId());
        res.push_back(c);
    }
    // Currently does not send sight ops, as the Sight ops for this type of
    // thing have not been discussed
}
