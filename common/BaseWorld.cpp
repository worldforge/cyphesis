// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <common/Setup.h>

#include "BaseWorld.h"

#include <rulesets/World.h>
#include <rulesets/EntityFactory.h>
#include <common/debug.h>
#include <common/const.h>
#include <common/globals.h>
#include <common/stringstream.h>

#include <strstream>

extern "C" {
    #include <stdio.h>
}

using Atlas::Message::Object;

static const bool debug_flag = false;

BaseWorld::BaseWorld(Entity & gWorld) : gameWorld(gWorld)
{
    // fullid = "world_0";
    // initTime = time(NULL) - timeoffset;
    // updateTime();
    // gameWorld.fullid = fullid;
    // gameWorld.world=this;
    // server.idDict[fullid]=&gameWorld;
    // eobjects[fullid]=&gameWorld;
    // perceptives.push_back(&gameWorld);
    // objectList.push_back(&gameWorld);
}

BaseWorld::~BaseWorld()
{
    delete &gameWorld;
}

#if 0
inline std::string BaseWorld::getId(std::string & name)
{
    std::stringstream buf;
    buf << name << "_" << ++nextId;
    std::string full_id = buf.str();
    size_t index;
    while ((index = full_id.find(' ', 0)) != std::string::npos) {
        full_id[index] = '_';
    }
    return full_id;
}

Entity * BaseWorld::addObject(Entity * obj)
{
    debug(cout << "BaseWorld::addObject(Entity *)" << endl << flush;);
    if (obj->fullid.empty()) {
        obj->fullid=getId(obj->name);
    }
    server.idDict[obj->fullid]=eobjects[obj->fullid]=obj;
    objectList.push_back(obj);
    if (!obj->location) {
        debug(cout << "set loc " << &gameWorld  << endl << flush;);
        obj->location.ref=&gameWorld;
        obj->location.coords=Vector3D(0,0,0);
        debug(cout << "loc set with ref " << obj->location.ref->fullid << endl << flush;);
    }
    if (obj->location.ref==&gameWorld) {
        debug(cout << "loc is world" << endl << flush;);
        gameWorld.contains.push_back(obj);
        gameWorld.contains.unique();
    }
    debug(cout << "Entity loc " << obj->location << endl << flush;);
    obj->world=this;
    if (obj->omnipresent) {
        omnipresentList.push_back(obj);
    }
    Setup * s = new Setup(Setup::Instantiate());
    s->SetTo(obj->fullid);
    s->SetFutureSeconds(-0.1);
    addOperationToQueue(*s, this);
    return (obj);
}

Entity * BaseWorld::addObject(const std::string & typestr, const Object & ent,
                                const std::string & id)
{
    debug(cout << "BaseWorld::addObject(std::string, ent)" << endl << flush;);
    Entity * obj;
    obj = EntityFactory::instance()->newThing(typestr, ent, eobjects);
    obj->fullid = id;
    return addObject(obj);
}

void BaseWorld::delObject(Entity * obj)
{
    // Remove object from contains of its real ref?
    if (obj->location.ref != NULL) {
        obj->location.ref->contains.remove(obj);
    }
    // Remove object from world just to make sure
    gameWorld.contains.remove(obj);
    omnipresentList.remove(obj);
    perceptives.remove(obj);
    objectList.remove(obj);
    eobjects.erase(obj->fullid);
    server.idDict.erase(obj->fullid);
}

inline const elist_t& BaseWorld::broadcastList(const RootOperation & op) const
{
    const Object::ListType & parents = op.GetParents();
    if ((parents.size() > 0) && (parents.front().IsString())) {
        const std::string & parent = parents.front().AsString();
        if ((parent == "sight") || (parent == "sound")) {
            return perceptives;
        }
    }
    return objectList;
}
#endif
