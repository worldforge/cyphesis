// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_METHODS_H
#define RULESETS_MEM_MAP_METHODS_H

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

using Atlas::Objects::Operation::Look;

#include "Entity.h"
#include "MemMap.h"
#include "Script.h"
#include <modules/Location.h>
#include <common/debug.h>

static const bool debug_flag = false;

inline Entity * MemMap::addObject(Entity * object)
{
    debug(std::cout << "MemMap::addObject " << object << " " << object->fullid
                    << std::endl << std::flush;);
    if (object != NULL) {
        things[object->fullid] = object;
    }

    debug( std::cout << things[object->fullid] << std::endl << std::flush;);
    debug( std::cout << this << std::endl << std::flush;);
    std::list<std::string>::const_iterator I;
    for(I = addHooks.begin(); I != addHooks.end(); I++) {
        script->hook(*I, object);
    }
    return object;
}

inline RootOperation * MemMap::lookId()
{
    debug( std::cout << "MemMap::lookId" << std::endl << std::flush;);
    if (additionsById.size() != 0) {
        std::string id = additionsById.front();
        additionsById.pop_front();
        Look * l = new Look(Look::Instantiate());
        //Object::MapType m;
        //m["id"] = Object(id);
        //l->SetArgs(Object::ListType(1, Object(m)));
        l->SetTo(id);
        return l;
    }
    return NULL;
}

inline Entity * MemMap::addId(const std::string & id)
{
    if (id.size() == 0) { return NULL; }
    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    additionsById.push_back(id);
    Object::MapType m;
    m["id"] = Object(std::string(id));
    Object obj(m);
    return add(obj);
}

inline void MemMap::del(const std::string & id)
{
    if (id.size() == 0) { return; }
    if (things.find(id) != things.end()) {
        Entity * obj = (Entity*)things[id];
        things.erase(id);
        std::list<std::string>::const_iterator I;
        for(I = deleteHooks.begin(); I != deleteHooks.end(); I++) {
            script->hook(*I, obj);
        }
        delete obj;
    }
}

inline Entity * MemMap::get(const std::string & id)
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.size() == 0) { return NULL; }
    if (things.find(id) != things.end()) {
        return (Entity*)things[id];
    }
    return NULL;
}

inline Entity * MemMap::getAdd(const std::string & id)
{
    debug( std::cout << "MemMap::getAdd" << std::endl << std::flush;);
    if (id.size() == 0) { return NULL; }
    Entity * obj = MemMap::get(id);
    if (obj != NULL) {
        return obj;
    }
    return addId(id);
}

#endif // RULESETS_MEM_MAP_METHODS_H
