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
    debug(std::cout << "MemMap::addObject " << object << " " << object->getId()
                    << std::endl << std::flush;);
    things[object->getId()] = object;

    debug( std::cout << things[object->getId()] << std::endl << std::flush;);
    debug( std::cout << this << std::endl << std::flush;);
    std::vector<std::string>::const_iterator I;
    for(I = addHooks.begin(); I != addHooks.end(); I++) {
        script->hook(*I, object);
    }
    return object;
}

inline RootOperation * MemMap::lookId()
{
    debug( std::cout << "MemMap::lookId" << std::endl << std::flush;);
    if (!additionsById.empty()) {
        const std::string & id = additionsById.front();
        Look * l = new Look(Look::Instantiate());
        Object::MapType m;
        m["id"] = Object(id);
        l->SetArgs(Object::ListType(1, Object(m)));
        l->SetTo(id);
        additionsById.pop_front();
        return l;
    }
    return NULL;
}

inline Entity * MemMap::addId(const std::string & id)
{
    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    additionsById.push_back(id);
    Object::MapType m;
    m["id"] = Object(id);
    return add(m);
}

inline void MemMap::del(const std::string & id)
{
    EntityDict::iterator I = things.find(id);
    if (I != things.end()) {
        Entity * obj = I->second;
        things.erase(I);
        std::vector<std::string>::const_iterator I;
        for(I = deleteHooks.begin(); I != deleteHooks.end(); I++) {
            script->hook(*I, obj);
        }
        delete obj;
    }
}

inline Entity * MemMap::get(const std::string & id)
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = things.find(id);
    if (I != things.end()) {
        return I->second;
    }
    return NULL;
}

inline Entity * MemMap::getAdd(const std::string & id)
{
    debug( std::cout << "MemMap::getAdd" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = things.find(id);
    if (I != things.end()) {
        return I->second;
    }
    return addId(id);
}

#endif // RULESETS_MEM_MAP_METHODS_H
