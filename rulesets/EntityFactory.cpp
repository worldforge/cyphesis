// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Thing.h"
#include "EntityFactory.h"
// #include "ThingShape.h"
#include "Python_API.h"

#include "ThingFactory.h"
#include "Character.h"
#include "Creator.h"
#include "Plant.h"
#include "Food.h"
#include "Stackable.h"
#include "Structure.h"

#include <common/const.h>
#include <common/debug.h>
#include <common/globals.h>

#include <server/ServerRouting.h>

#include "Character.h"
#include "Creator.h"

static const bool debug_flag = false;

using Atlas::Message::Object;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory()
{
    factories["thing"] = new ThingFactory<Thing>();
    factories["character"] = new ThingFactory<Character>();
    factories["creator"] = new ThingFactory<Creator>();
    factories["plant"] = new ThingFactory<Plant>();
    factories["food"] = new ThingFactory<Food>();
    factories["stackable"] = new ThingFactory<Stackable>();
    factories["structure"] = new ThingFactory<Structure>();
}

void EntityFactory::readRuleset(const string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

Thing * EntityFactory::newThing(const string & type,const Object & ent, WorldRouter * svr)
{
    if (!ent.IsMap()) {
         debug( cout << "Entity is not a map" << endl << flush;);
    }
    Thing * thing = NULL;
    string py_package;
    fdict_t::iterator I = factories.find(type);
    if (type.size() == 0) {
        thing = new Thing();
    } else if (I != factories.end()) {
        thing = I->second->newThing();
    } else {
        fdict_t::const_iterator J;
        for(J = factories.begin(); J != factories.end(); J++) {
            const string & f_type = J->first;
            if (global_conf->findItem(f_type, type)) {
                thing = J->second->newThing();
                py_package = global_conf->getItem(f_type, type);
            }
        }
    }
    if (thing == NULL) {
        thing = new Thing();
    }
    debug( cout << "[" << type << " " << thing->name << "]" << endl << flush;);
    thing->type = type;
    // Sort out python object
    if (py_package.size() != 0) {
        Create_PyThing(thing, py_package, type);
    }
    const Object::MapType & entmap = ent.AsMap();
    Object::MapType::const_iterator K = entmap.find("name");
    if ((K != entmap.end()) && K->second.IsString()) {
        thing->name = K->second.AsString();
    } else {
        debug( cout << "Got no name" << endl << flush;);
    }
    thing->merge(entmap);
    // Get location from entity, if it is present
    thing->getLocation(entmap, svr->eobjects);
    return thing;
}

void EntityFactory::flushFactories()
{
    fdict_t::const_iterator I = factories.begin();
    for (; I != factories.end(); I++) {
        delete I->second;
    }
}
