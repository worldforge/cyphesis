// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Delete.h>

// A stackable object, ie one which can represent multiple object of the
// same type. Used for things like coins.

#include "Stackable.h"
#include "Script.h"

using Atlas::Message::Object;

Stackable::Stackable() : num(1)
{
    subscribe("combine", OP_COMBINE);
    subscribe("divide", OP_DIVIDE);
}

Stackable::~Stackable()
{
}

const Object & Stackable::operator[](const std::string & aname)
{
    if (aname == "num") {
        attributes[aname] = Object(num);
    }
    return Thing::operator[](aname);
}

void Stackable::set(const std::string & aname, const Object & attr)
{
    if ((aname == "num") && attr.IsInt()) {
        num = attr.AsInt();
    } else {
        Thing::set(aname, attr);
    }
}

void Stackable::addToObject(Object::MapType & omap) const
{
    if (num != 1) {
        omap["num"] = Object(num);
    }
    Entity::addToObject(omap);
}

OpVector Stackable::CombineOperation(const Combine & op)
{
    OpVector res;
    if (script->Operation("combine", op, res) != 0) {
        return res;
    }
    const Object::ListType & args = op.GetArgs();
    for(Object::ListType::const_iterator I= args.begin(); I!= args.end(); I++) {
        const std::string & id = I->AsMap().find("id")->second.AsString();
        if (id == getId()) { continue; }
        Entity * ent = world->getObject(id);
        if (ent == NULL) { continue; }
        Stackable * obj = dynamic_cast<Stackable *>(ent);
        if (obj == NULL) { continue; }
        if (obj->type != type) { continue; }
        num = num + obj->num;

        Delete * d = new Delete(Delete::Instantiate());
        Object::MapType dent;
        dent["id"] = id;
        d->SetTo(id);
        d->SetArgs(Object::ListType(1,dent));
        res.push_back(d);
    }
    return res;
    // FIXME DO we need to send a sight?
}

OpVector Stackable::DivideOperation(const Divide & op)
{
    OpVector res;
    if (script->Operation("divide", op, res) != 0) {
        return res;
    }
    const Object::ListType & args = op.GetArgs();
    for(Object::ListType::const_iterator I= args.begin(); I!= args.end(); I++) {
        const Object::MapType & ent = I->AsMap();
        int new_num = 1;
        Object::MapType::const_iterator J = ent.find("num");
        if (J != ent.end()) {
            if (J->second.IsInt()) { new_num = J->second.AsInt(); }
        }
        if (num <= new_num) { continue; }
        
        Object::MapType new_ent;
        Object::ListType parents(1,type);
        new_ent["parents"] = parents;
        new_ent["num"] = new_num;
        Create * c = new Create( Create::Instantiate());
        c->SetArgs(Object::ListType(1,new_ent));
        c->SetTo(getId());
        res.push_back(c);
    }
    return res;
    // FIXME DO we need to send a sight?
}
