// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

using Atlas::Objects::Operation::Look;

#include <modules/Location.h>
#include <common/debug.h>

#include "Entity.h"
#include "MemMap.h"
#include "MemMap_methods.h"
#include "Script.h"

Entity * MemMap::add(const Object & entity)
{
    debug( cout << "MemMap::add" << endl << flush;);
    if (!entity.IsMap()) {
        return NULL;
    }
    Object::MapType entmap = entity.AsMap();
    if ((entmap.find("id") == entmap.end()) ||
        (entmap["id"].AsString().size() == 0)) {
        return NULL;
    }
    if (get(entmap["id"].AsString())) {
        return update(entity);
    }
    Entity * thing = new Entity;
    thing->fullid = entmap["id"].AsString();
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    }
    if (entmap.find("type") != entmap.end() && entmap["type"].IsString()) {
        thing->type = entmap["type"].AsString();
    }
    thing->merge(entmap);
    if (entmap.find("loc") != entmap.end()) {
        getAdd(entmap["loc"].AsString());
    }
    thing->getLocation(entmap, things);
    return addObject(thing);
}

Entity * MemMap::update(const Object & entity)
{
    debug( cout << "MemMap::update" << endl << flush;);
    if (!entity.IsMap()) {
        return NULL;
    }
    Object::MapType entmap = entity.AsMap();
    if (entmap.find("id") == entmap.end()) {
        return NULL;
    }
    string & id = entmap["id"].AsString();
    if (id.size() == 0) {
        return NULL;
    }
    debug( cout << " updating " << id << endl << flush;);
    if (things.find(id) == things.end()) {
        return add(entity);
    }
    debug( cout << " " << id << " has already been spotted" << endl << flush;);
    Entity * thing = (Entity*)things[id];
    debug( cout << " got " << thing << endl << flush;);
    // I am not sure what the deal is with all the "needTrueValue stuff
    // below yet. FIXME find out exactly what is required.
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    }
    if (entmap.find("type") != entmap.end() && entmap["type"].IsString()) {
        thing->type = entmap["type"].AsString();
    }
    debug( cout << " got " << thing << endl << flush;);
    thing->merge(entmap);
    thing->getLocation(entmap,things);
    //needTrueValue=["type","contains","instance","id","location","stamp"];
    //for (/*(key,value) in entity.__dict__.items()*/) {
        //if (value or not key in needTrueValue) {
            //setattr(obj,key,value);
        //}
    //}
    list<string>::const_iterator I;
    for(I = updateHooks.begin(); I != updateHooks.end(); I++) {
        script->hook(*I, thing);
    }
    //for (/*hook in MemMap::update_hooks*/) {
        //hook(obj);
    //}
    return thing;
}

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
