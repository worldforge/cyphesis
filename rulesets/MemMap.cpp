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

void MemMap::addContents(const Object::MapType & entmap)
{
    Object::MapType::const_iterator I = entmap.find("contains");
    if ((I == entmap.end()) || (!I->second.IsList())) {
        return;
    }
    const Object::ListType & contlist = I->second.AsList();
    Object::ListType::const_iterator J = contlist.begin();
    for(;J != contlist.end(); J++) {
        if (!J->IsString()) {
            continue;
        }
        getAdd(J->AsString());
    }
}

Entity * MemMap::add(const Object & entity)
{
    debug( std::cout << "MemMap::add" << std::endl << std::flush;);
    if (!entity.IsMap()) {
        return NULL;
    }
    const Object::MapType & entmap = entity.AsMap();
    Object::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || (I->second.AsString().empty())) {
        return NULL;
    }
    const std::string & id = I->second.AsString();
    if (get(id)) {
        return update(entity);
    }
    Entity * thing = new Entity;
    thing->setId(id);
    I = entmap.find("name");
    if ((I != entmap.end()) && I->second.IsString()) {
        thing->setName(I->second.AsString());
    }
    I = entmap.find("type");
    if ((I != entmap.end()) && I->second.IsString()) {
        thing->setType(I->second.AsString());
    }
    thing->merge(entmap);
    I = entmap.find("loc");
    if ((I != entmap.end()) && I->second.IsString()) {
        getAdd(I->second.AsString());
    }
    thing->getLocation(entmap, things);
    addContents(entmap);
    return addObject(thing);
}

Entity * MemMap::update(const Object & entity)
{
    debug( std::cout << "MemMap::update" << std::endl << std::flush;);
    if (!entity.IsMap()) {
        return NULL;
    }
    const Object::MapType & entmap = entity.AsMap();
    Object::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || !I->second.IsString()) {
        return NULL;
    }
    const std::string & id = I->second.AsString();
    if (id.empty()) {
        return NULL;
    }
    debug( std::cout << " updating " << id << std::endl << std::flush;);
    edict_t::iterator J = things.find(id);
    if (J == things.end()) {
        return add(entity);
    }
    debug( std::cout << " " << id << " has already been spotted" << std::endl << std::flush;);
    Entity * thing = J->second;
    debug( std::cout << " got " << thing << std::endl << std::flush;);
    // I am not sure what the deal is with all the "needTrueValue stuff
    // below yet. FIXME find out exactly what is required.
    I = entmap.find("name");
    if (I != entmap.end() && I->second.IsString()) {
        thing->setName(I->second.AsString());
    }
    I = entmap.find("type");
    if (I != entmap.end() && I->second.IsString()) {
        thing->setType(I->second.AsString());
    }
    debug( std::cout << " got " << thing << std::endl << std::flush;);
    thing->merge(entmap);
    thing->getLocation(entmap,things);
    addContents(entmap);
    //needTrueValue=["type","contains","instance","id","location","stamp"];
    //for (/*(key,value) in entity.__dict__.items()*/) {
        //if (value or not key in needTrueValue) {
            //setattr(obj,key,value);
        //}
    //}
    std::list<std::string>::const_iterator K;
    for(K = updateHooks.begin(); K != updateHooks.end(); K++) {
        script->hook(*K, thing);
    }
    //for (/*hook in MemMap::update_hooks*/) {
        //hook(obj);
    //}
    return thing;
}

std::list<Entity *> MemMap::findByType(const std::string & what)
{
    std::list<Entity *> res;
    edict_t::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        Entity * item = (Entity *)I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->getType() == what) {
            res.push_back((Entity*)I->second);
        }
    }
    return res;
}

std::list<Entity *> MemMap::findByLocation(const Location & loc, double radius)
{
    std::list<Entity *> res;
    edict_t::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        const Location & oloc = I->second->location;
        if (!loc || !oloc) {
            continue;
        }
        if ((oloc.ref->getId() == loc.ref->getId()) &&
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
