// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

using Atlas::Objects::Operation::Look;

#include "Entity.h"
#include "MemMap.h"
#include <modules/Location.h>
#include "common/debug.h"

static const bool debug_flag = false;

list<Entity *> MemMap::findByType(const string & what)
{
    list<Entity *> res;
    dict_t::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        Entity * item = (Entity *)I->second;
        debug( cout << "F" << what << ":" << item->type << ":" << item->fullid << endl << flush;);
        if (item->type == what) {
            res.push_back((Entity*)I->second);
        }
    }
    return res;
}

list<Entity *> MemMap::findByLocation(const Location & loc, double radius)
{
    list<Entity *> res;
    dict_t::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        const Location & oloc = I->second->location;
        if (!loc || !oloc) {
            continue;
        }
        if ((oloc.ref->fullid == loc.ref->fullid) &&
            (loc.coords.distance(oloc.coords) < radius)) {
            res.push_back((Entity*)I->second);
        }
    }
    return res;
}
