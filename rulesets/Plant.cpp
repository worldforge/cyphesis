// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>

#include <common/Tick.h>

#include "Plant.h"
#include "Script.h"

#include <common/const.h>
#include <common/random.h>

using Atlas::Message::Object;

Plant::Plant() : fruits(0), radius(1), fruitName("seed")
{
    // Default to a 1m cube
    cout << "CREATING PLANT" << endl << flush;
    location.bbox = Vector3D(0.5, 0.5, 0.5);
    location.bmedian = Vector3D(0, 0, 0.5);
}

Plant::~Plant()
{
}

const Object & Plant::operator[](const string & aname)
{
    if (aname == "fruits") {
        attributes[aname] = Object(fruits);
    } else if (aname == "radius") {
        attributes[aname] = Object(radius);
    } else if (aname == "fruitName") {
        attributes[aname] = Object(fruitName);
    } else if (aname == "fruitChance") {
        attributes[aname] = Object(fruitChance);
    } else if (aname == "sizeAdult") {
        attributes[aname] = Object(sizeAdult);
    }
    return Thing::operator[](aname);
}

void Plant::set(const string & aname, const Object & attr)
{
    if ((aname == "fruits") && attr.IsInt()) {
        fruits = attr.AsInt();
    } else if ((aname == "radius") && attr.IsInt()) {
        radius = attr.AsInt();
    } else if ((aname == "fruitName") && attr.IsString()) {
        fruitName = attr.AsString();
    } else if ((aname == "fruitChance") && attr.IsInt()) {
        fruitChance = attr.AsInt();
    } else if ((aname == "sizeAdult") && attr.IsNum()) {
        sizeAdult = attr.AsNum();
    } else {
        Thing::set(aname, attr);
    }
}

int Plant::dropFruit(oplist & res)
{
    if (fruits < 1) { return 0; }
    int drop = min(fruits, randint(minuDrop, maxuDrop));
    fruits = fruits - drop;
    double height = location.bbox.Z(); 
    for(int i = 0; i < drop; i++) {
        double rx = location.coords.X()+uniform(height*radius, -height*radius);
        double ry = location.coords.X()+uniform(height*radius, -height*radius);
        Object::MapType fmap;
        fmap["name"] = fruitName;
        fmap["parents"] = Object::ListType(1,fruitName);
        Location floc(location.ref, Vector3D(rx, ry, 0));
        floc.addToObject(fmap);
        RootOperation * create = new Create(Create::Instantiate());
        create->SetArgs(Object::ListType(1, fmap));
        res.push_back(create);
    }
    return drop;
}

oplist Plant::TickOperation(const Tick & op)
{
    oplist res;
    script->Operation("tick", op, res);
    RootOperation * tickOp = new Tick(Tick::Instantiate());
    tickOp->SetTo(fullid);
    tickOp->SetFutureSeconds(consts::basic_tick * speed);
    res.push_back(tickOp);
    int dropped = dropFruit(res);
    if (location.bbox.Z() > sizeAdult) {
        if (randint(1, fruitChance) == 1) {
            fruits++;
            dropped--;
        }
    }
    if (dropped != 0) {
        RootOperation * set = new Set(Set::Instantiate());
        Object::MapType pmap;
        pmap["id"] = fullid;
        pmap["fruits"] = fruits;
        set->SetArgs(Object::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}
