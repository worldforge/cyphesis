// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
    debug(cout << "MemMap::addObject " << object << " " << object->fullid
               << endl << flush;);
    if (object != NULL) {
        things[object->fullid] = object;
    }

    debug( cout << things[object->fullid] << endl << flush;);
    debug( cout << this << endl << flush;);
    list<string>::const_iterator I;
    for(I = addHooks.begin(); I != addHooks.end(); I++) {
        script->hook(*I, object);
    }
    return object;
}

inline RootOperation * MemMap::lookId()
{
    debug( cout << "MemMap::lookId" << endl << flush;);
    if (additionsById.size() != 0) {
        string id = additionsById.front();
        additionsById.pop_front();
        Look * l = new Look(Look::Instantiate());
        //Object::MapType m;
        //m["id"] = Object(id);
        //l->SetArgs(Object::ListType(1, Object(m)));
        l->SetTo(id);
        return l;
    }
    return(NULL);
}

inline Entity * MemMap::addId(const string & id)
{
    if (id.size() == 0) { return NULL; }
    debug( cout << "MemMap::add_id" << endl << flush;);
    additionsById.push_back(id);
    Object::MapType m;
    m["id"] = Object(string(id));
    Object obj(m);
    return add(obj);
}

inline void MemMap::del(const string & id)
{
    if (id.size() == 0) { return; }
    if (things.find(id) != things.end()) {
        Entity * obj = (Entity*)things[id];
        things.erase(id);
        list<string>::const_iterator I;
        for(I = deleteHooks.begin(); I != deleteHooks.end(); I++) {
            script->hook(*I, obj);
        }
        delete obj;
    }
}

inline Entity * MemMap::get(const string & id)
{
    debug( cout << "MemMap::get" << endl << flush;);
    if (id.size() == 0) { return NULL; }
    if (things.find(id) != things.end()) {
        return (Entity*)things[id];
    }
    return(NULL);
}

inline Entity * MemMap::getAdd(const string & id)
{
    debug( cout << "MemMap::getAdd" << endl << flush;);
    if (id.size() == 0) { return NULL; }
    Entity * obj = MemMap::get(id);
    if (obj != NULL) {
        return obj;
    }
    return addId(id);
}
