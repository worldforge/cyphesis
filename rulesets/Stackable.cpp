// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Login.h>

#include <common/const.h>

// A stackable object, ie one which can represent multiple object of the
// same type. Used for things like coins.

#include "Stackable.h"
#include "Script.h"

using Atlas::Message::Object;

Stackable::Stackable() : num(1)
{
}

const Object & Stackable::operator[](const string & aname)
{
    if (aname == "num") {
        attributes[aname] = Object(num);
    }
    return Thing::operator[](aname);
}

void Stackable::set(const string & aname, const Object & attr)
{
    if ((aname == "num") && attr.IsInt()) {
        num = attr.AsInt();
    } else {
        Thing::set(aname, attr);
    }
}

void Stackable::addToObject(Object * obj) const
{
    Object::MapType & omap = obj->AsMap();
    if (num != 1) {
        omap["num"] = Object(num);
    }
    Thing::addToObject(obj);
}

oplist Stackable::Operation(const Combine & op)
{
    oplist res;
    if (script->Operation("combine", op, res) != 0) {
        return(res);
    }
    Object::ListType args = op.GetArgs();
    for(Object::ListType::iterator I = args.begin(); I != args.end(); I++) {
        const string & id = I->AsMap()["id"].AsString();
        if (id == fullid) { continue; }
        Stackable * obj = (Stackable*)world->getObject(id);
        if (!obj->inGame) { continue; }
        if (obj->type != type) { continue; }
        num = num + obj->num;

        Delete * d = new Delete();
        *d = Delete::Instantiate();
        Object::MapType dent;
        dent["id"] = id;
        d->SetTo(id);
        d->SetArgs(Object::ListType(1,dent));
        res.push_back(d);
    }
    return res;
    // FIXME DO we need to send a sight?
}

oplist Stackable::Operation(const Divide & op)
{
    oplist res;
    if (script->Operation("divide", op, res) != 0) {
        return(res);
    }
    Object::ListType args = op.GetArgs();
    for(Object::ListType::iterator I = args.begin(); I != args.end(); I++) {
        Object::MapType & ent = I->AsMap();
        int new_num = 1;
        if (ent.find("num") != ent.end()) {
            if (ent["num"].IsInt()) { new_num = ent["num"].AsInt(); }
        }
        if (num <= new_num) { continue; }
        
        Object::MapType new_ent;
        Object::ListType parents(1,type);
        new_ent["parents"] = parents;
        new_ent["num"] = new_num;
        Create * c = new Create();
        *c = Create::Instantiate();
        c->SetArgs(Object::ListType(1,new_ent));
        c->SetTo(fullid);
        res.push_back(c);
    }
    return res;
    // FIXME DO we need to send a sight?
}
