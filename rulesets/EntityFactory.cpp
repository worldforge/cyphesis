// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Thing.h"
#include "EntityFactory.h"
// #include "ThingShape.h"
#include "Python_API.h"

#include "ThingFactory.h"
#include "CharacterFactory.h"
#include "CreatorFactory.h"
#include "PlantFactory.h"
#include "FoodFactory.h"
#include "StackFactory.h"
#include "StructFactory.h"

#include <server/WorldRouter.h>

#include <common/const.h>
#include <common/debug.h>

#include "Character.h"
#include "Creator.h"

static const bool debug_flag = false;

using Atlas::Message::Object;

// map<string, ThingShape *> * ThingShape::dict = NULL;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory()
{
    factories["thing"] = ThingFactory::instance();
    factories["character"] = CharacterFactory::instance();
    factories["creator"] = CreatorFactory::instance();
    factories["plant"] = PlantFactory::instance();
    factories["food"] = FoodFactory::instance();
    factories["stackable"] = StackFactory::instance();
    factories["structure"] = StructureFactory::instance();
}

void EntityFactory::readRuleset(const string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

Thing * EntityFactory::newThing(const string & type,const Object & ent, Routing * svr)
{
    if (!ent.IsMap()) {
         debug( cout << "Entity is not a map" << endl << flush;);
    }
    Object::MapType entmap = ent.AsMap();
    Thing * thing = NULL;
    string py_package;
    if (type.size() == 0) {
        thing = new Thing();
    } else if (factories.find(type) != factories.end()) {
        thing = factories[type]->newThing();
    } else {
        std::map<std::string, ThingFactory *>::const_iterator I;
        for(I = factories.begin(); I != factories.end(); I++) {
            const string & f_type = I->first;
            if (global_conf->findItem(f_type, type)) {
                thing = I->second->newThing();
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
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    } else {
        debug( cout << "Got no name" << endl << flush;);
    }
    thing->merge(entmap);
    // Get location from entity, if it is present
    thing->getLocation(entmap, svr->fobjects);
    return(thing);
}
