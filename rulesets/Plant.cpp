// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>

#include <common/Tick.h>
#include <common/const.h>
#include <common/random.h>

#include "Plant.h"
#include "Script.h"

using Atlas::Message::Object;

Plant::Plant() : fruits(0), radius(1), fruitname("seed")
{
    // Default to a 1m cube
    cout << "CREATING PLANT" << endl << flush;
    location.bbox = Vector3D(0.5, 0.5, 0.5);
    location.bmedian = Vector3D(0, 0, 0.5);
}

const Object & Plant::operator[](const string & aname)
{
    if (aname == "fruits") {
        attributes[aname] = Object(fruits);
    } else if (aname == "radius") {
        attributes[aname] = Object(radius);
    } else if (aname == "fruitname") {
        attributes[aname] = Object(fruitname);
    } else if (aname == "fruitchance") {
        attributes[aname] = Object(fruitchance);
    } else if (aname == "sizeadult") {
        attributes[aname] = Object(sizeadult);
    }
    return Thing::operator[](aname);
}

void Plant::set(const string & aname, const Object & attr)
{
    if ((aname == "fruits") && attr.IsInt()) {
        fruits = attr.AsInt();
    } else if ((aname == "radius") && attr.IsInt()) {
        radius = attr.AsInt();
    } else if ((aname == "fruitname") && attr.IsString()) {
        fruitname = attr.AsString();
    } else if ((aname == "fruitchance") && attr.IsInt()) {
        fruitchance = attr.AsInt();
    } else if ((aname == "sizeadult") && attr.IsNum()) {
        sizeadult = attr.AsNum();
    } else {
        Thing::set(aname, attr);
    }
}

inline int Plant::drop_fruit(oplist & res)
{
    if (fruits < 1) { return 0; }
    int drop = min(fruits, randint(minudrop, maxudrop));
    fruits = fruits - drop;
    double height = location.bbox.Z(); 
    for(int i = 0; i < drop; i++) {
        double rx = location.coords.X()+uniform(height*radius, -height*radius);
        double ry = location.coords.X()+uniform(height*radius, -height*radius);
        Object::MapType fmap;
        fmap["name"] = fruitname;
        fmap["parents"] = Object::ListType(1,"fruitname");
        Location floc(location.ref, Vector3D(rx, ry, 0));
        Object fruit(fmap);
        floc.addObject(&fruit);
        RootOperation * create = new Create();
        *create = Create::Instantiate();
        create->SetArgs(Object::ListType(1, fruit));
        res.push_back(create);
    }
    return drop;
}

oplist Plant::Operation(const Tick & op)
{
    oplist res;
    script->Operation("tick", op, res);
    RootOperation * tickOp = new Tick();
    *tickOp = Tick::Instantiate();
    tickOp->SetTo(fullid);
    tickOp->SetFutureSeconds(consts::basic_tick * speed);
    res.push_back(tickOp);
    int dropped = drop_fruit(res);
    if (location.bbox.Z() > sizeadult) {
        if (randint(1, fruitchance) == 1) {
            fruits++;
            dropped--;
        }
    }
    if (dropped != 0) {
        RootOperation * set = new Set();
        *set = Set::Instantiate();
        Object::MapType pmap;
        pmap["id"] = fullid;
        pmap["fruits"] = fruits;
        set->SetArgs(Object::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}
