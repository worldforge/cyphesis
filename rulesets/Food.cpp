// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Food.h"

#include <common/Eat.h>
#include <common/Fire.h>
#include <common/Nourish.h>

Food::Food()
{
    attributes["cooked"] = 0;
    weight = 1;
}

oplist Food::Operation(const Eat & op)
{
    oplist res;
    if (script_Operation("eat", op, res) != 0) {
        return(res);
    }
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    self_ent["status"] = -1;

    Set * s = new Set();
    *s = Set::Instantiate();
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    const string & to = op.GetFrom();
    Object::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["weight"] = weight;
    Nourish * n = new Nourish();
    *n = Nourish::Instantiate();
    n->SetTo(to);
    n->SetArgs(Object::ListType(1,nour_ent));

    oplist res2;
    res2[0] = s;
    res2[1] = n;
    return res2;
}

oplist Food::Operation(const Fire & op)
{
    oplist res;
    if (script_Operation("fire", op, res) != 0) {
        return(res);
    }
    double cooked;
    if (attributes.find("cooked") == attributes.end()) {
        cooked = attributes["cooked"].AsNum();
    }
    Object::MapType fire_ent = op.GetArgs().front().AsMap();
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    // Currently this cooks pretty quick, and at the same speed for
    // everything. No mechanism for this yet.
    self_ent["cooked"] = cooked - (fire_ent["status"].AsNum()/weight);

    Set * s = new Set();
    *s = Set::Instantiate();
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    return oplist(1,s);
}

