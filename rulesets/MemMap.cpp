// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Entity.h"
#include "MemMap.h"
#include "MemMap_methods.h"
#include "Script.h"

#include <modules/Location.h>
#include <common/debug.h>

#include <Atlas/Objects/Operation/Look.h>

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

Entity * MemMap::add(const Object::MapType & entmap)
{
    debug( std::cout << "MemMap::add" << std::endl << std::flush;);
    Object::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || (I->second.AsString().empty())) {
        return NULL;
    }
    const std::string & id = I->second.AsString();
    if (find(id)) {
        return update(entmap);
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

Entity * MemMap::update(const Object::MapType & entmap)
{
    debug( std::cout << "MemMap::update" << std::endl << std::flush;);
    Object::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || !I->second.IsString()) {
        return NULL;
    }
    const std::string & id = I->second.AsString();
    if (id.empty()) {
        return NULL;
    }
    debug( std::cout << " updating " << id << std::endl << std::flush;);
    EntityDict::iterator J = things.find(id);
    if (J == things.end()) {
        return add(entmap);
    }
    debug( std::cout << " " << id << " has already been spotted" << std::endl << std::flush;);
    Entity * thing = J->second;
    debug( std::cout << " got " << thing << std::endl << std::flush;);
    I = entmap.find("name");
    if (I != entmap.end() && I->second.IsString()) {
        thing->setName(I->second.AsString());
    }
    I = entmap.find("type");
    if (I != entmap.end() && I->second.IsString()) {
        thing->setType(I->second.AsString());
    }
    debug( std::cout << " got " << thing << std::endl << std::flush;);
    // It is important that the entity is not mutated here. Ie, an update
    // should not affect its type, contain or id, and location and
    // stamp should be updated with accurate information
    thing->merge(entmap);
    thing->getLocation(entmap,things);
    addContents(entmap);
    std::vector<std::string>::const_iterator K;
    for(K = updateHooks.begin(); K != updateHooks.end(); K++) {
        script->hook(*K, thing);
    }
    return thing;
}

EntityVector MemMap::findByType(const std::string & what)
{
    EntityVector res;
    EntityDict::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        Entity * item = I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->getType() == what) {
            res.push_back(I->second);
        }
    }
    return res;
}

EntityVector MemMap::findByLocation(const Location & loc, double radius)
{
    EntityVector res;
    EntityDict::const_iterator I;
    for(I = things.begin(); I != things.end(); I++) {
        const Location & oloc = I->second->location;
        if (!loc.isValid() || !oloc.isValid()) {
            continue;
        }
        if ((oloc.ref->getId() == loc.ref->getId()) &&
            (loc.coords.relativeDistance(oloc.coords) < (radius * radius))) {
            res.push_back(I->second);
        }
    }
    return res;
}

const Object MemMap::asObject()
{
    Object::MapType omap;
    EntityDict::iterator I = things.begin();
    for(;I != things.end(); I++) {
        omap[I->first] = I->second->asObject();
    }
    return Object(omap);
}

void MemMap::flushMap()
{
    EntityDict::const_iterator I = things.begin();
    for (; I != things.end(); I++) {
        delete I->second;
    }
}
