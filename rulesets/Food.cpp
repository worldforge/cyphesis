// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Set.h>

#include <common/Eat.h>
#include <common/Fire.h>
#include <common/Nourish.h>

#include "Food.h"
#include "Script.h"

Food::Food()
{
    attributes["cooked"] = 0;
    attributes["burn_speed"] = 0.1;
    weight = 1;
}

Food::~Food()
{
}

oplist Food::Operation(const Eat & op)
{
    oplist res;
    if (script->Operation("eat", op, res) != 0) {
        return res;
    }
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    self_ent["status"] = -1;

    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    const string & to = op.GetFrom();
    Object::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["weight"] = weight;
    Nourish * n = new Nourish(Nourish::Instantiate());
    n->SetTo(to);
    n->SetArgs(Object::ListType(1,nour_ent));

    oplist res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

oplist Food::Operation(const Fire & op)
{
    oplist res;
    if (script->Operation("fire", op, res) != 0) {
        return res;
    }
    double cooked = 0;
    Object::MapType::iterator I = attributes.find("cooked");
    if (I != attributes.end()) {
        cooked = I->second.AsNum();
    }
    const Object::MapType & fire_ent = op.GetArgs().front().AsMap();
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    // Currently this cooks pretty quick, and at the same speed for
    // everything. No mechanism for this yet.
    double fire_size = fire_ent.find("status")->second.AsNum();
    self_ent["cooked"] = cooked + (fire_size/weight);
    if (cooked > 1.0) {
        self_ent["status"] = status - (attributes["burn_speed"].AsNum()) * fire_size;
    }

    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    return oplist(1,s);
}

