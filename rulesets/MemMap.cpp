// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>

#include <modules/Location.h>
#include <common/debug.h>

#include "Entity.h"
#include "MemMap.h"
#include "MemMap_methods.h"
#include "Script.h"

using Atlas::Objects::Operation::Look;

Entity * MemMap::add(const Object & entity)
{
    debug( cout << "MemMap::add" << endl << flush;);
    if (!entity.IsMap()) {
        return NULL;
    }
    const Object::MapType & entmap = entity.AsMap();
    Object::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || (I->second.AsString().size() == 0)) {
        return NULL;
    }
    const string & id = I->second.AsString();
    if (get(id)) {
        return update(entity);
    }
    Entity * thing = new Entity;
    thing->fullid = id;
    I = entmap.find("name");
    if ((I != entmap.end()) && I->second.IsString()) {
        thing->name = I->second.AsString();
    }
    I = entmap.find("type");
    if ((I != entmap.end()) && I->second.IsString()) {
        thing->type = I->second.AsString();
    }
    thing->merge(entmap);
    I = entmap.find("loc");
    if (I != entmap.end()) {
        getAdd(I->second.AsString());
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
    const Object::MapType & entmap = entity.AsMap();
    Object::MapType::const_iterator I = entmap.find("id");
    if (I == entmap.end()) {
        return NULL;
    }
    const string & id = I->second.AsString();
    if (id.size() == 0) {
        return NULL;
    }
    debug( cout << " updating " << id << endl << flush;);
    edict_t::iterator J = things.find(id);
    if (J == things.end()) {
        return add(entity);
    }
    debug( cout << " " << id << " has already been spotted" << endl << flush;);
    Entity * thing = J->second;
    debug( cout << " got " << thing << endl << flush;);
    // I am not sure what the deal is with all the "needTrueValue stuff
    // below yet. FIXME find out exactly what is required.
    I = entmap.find("name");
    if (I != entmap.end() && I->second.IsString()) {
        thing->name = I->second.AsString();
    }
    I = entmap.find("type");
    if (I != entmap.end() && I->second.IsString()) {
        thing->type = I->second.AsString();
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
    list<string>::const_iterator K;
    for(K = updateHooks.begin(); K != updateHooks.end(); K++) {
        script->hook(*K, thing);
    }
    //for (/*hook in MemMap::update_hooks*/) {
        //hook(obj);
    //}
    return thing;
}

list<Entity *> MemMap::findByType(const string & what)
{
    list<Entity *> res;
    edict_t::const_iterator I;
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
    edict_t::const_iterator I;
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

const Object MemMap::asObject()
{
    Object::MapType omap;
    edict_t::iterator I = things.begin();
    for(;I != things.end(); I++) {
        omap[I->first] = I->second->asObject();
    }
    return Object(omap);
}

void MemMap::flushMap()
{
    edict_t::const_iterator I = things.begin();
    for (; I != things.end(); I++) {
        delete I->second;
    }
}
