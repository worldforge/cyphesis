// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Plant.h"

#include "Script.h"

#include <common/const.h>
#include <common/random.h>

#include <common/Tick.h>

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>

Plant::Plant() : fruits(0), radius(1), fruitName("seed")
{
    // Default to a 1m cube
    location.bBox = BBox(Vector3D(-0.5, -0.5, 0), Vector3D(0.5, 0.5, 1));

    subscribe("tick", OP_TICK);
}

Plant::~Plant()
{
}

const Fragment Plant::get(const std::string & aname) const
{
    if (aname == "fruits") {
        return fruits;
    } else if (aname == "radius") {
        return radius;
    } else if (aname == "fruitName") {
        return fruitName;
    } else if (aname == "fruitChance") {
        return fruitChance;
    } else if (aname == "sizeAdult") {
        return sizeAdult;
    }
    return Thing::get(aname);
}

void Plant::set(const std::string & aname, const Fragment & attr)
{
    if ((aname == "fruits") && attr.IsInt()) {
        fruits = attr.AsInt();
	update_flags != a_fruit;
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

int Plant::dropFruit(OpVector & res)
{
    if (fruits < 1) { return 0; }
    int drop = std::min(fruits, randint(minuDrop, maxuDrop));
    fruits = fruits - drop;
    double height = location.bBox.farPoint().Z(); 
    for(int i = 0; i < drop; i++) {
        double rx = location.coords.X()+uniform(height*radius, -height*radius);
        double ry = location.coords.X()+uniform(height*radius, -height*radius);
        Fragment::MapType fmap;
        fmap["name"] = fruitName;
        fmap["parents"] = Fragment::ListType(1,fruitName);
        Location floc(location.ref, Vector3D(rx, ry, 0));
        floc.addToObject(fmap);
        RootOperation * create = new Create(Create::Instantiate());
        create->SetArgs(Fragment::ListType(1, fmap));
        res.push_back(create);
    }
    return drop;
}

OpVector Plant::TickOperation(const Tick & op)
{
    OpVector res;
    script->Operation("tick", op, res);
    RootOperation * tickOp = new Tick(Tick::Instantiate());
    tickOp->SetTo(getId());
    tickOp->SetFutureSeconds(consts::basic_tick * speed);
    res.push_back(tickOp);
    int dropped = dropFruit(res);
    if (location.bBox.farPoint().Z() > sizeAdult) {
        if (randint(1, fruitChance) == 1) {
            fruits++;
            dropped--;
        }
    }
    if (dropped != 0) {
        RootOperation * set = new Set(Set::Instantiate());
        Fragment::MapType pmap;
        pmap["id"] = getId();
        pmap["fruits"] = fruits;
        set->SetArgs(Fragment::ListType(1,pmap));
        res.push_back(set);
    }
    return res;
}
