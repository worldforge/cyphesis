// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include <varconf/Config.h>

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

#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

#include "Character.h"
#include "Creator.h"

static const bool debug_flag = false;

using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Message::Object;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory()
{
    // This class can only have one instance, so a Factory is not installed
    installFactory("game_entity", "world", NULL);

    installFactory("game_entity", "thing", new ThingFactory<Thing>());
    installFactory("thing", "character", new ThingFactory<Character>());
    installFactory("character", "creator", new ThingFactory<Creator>());
    installFactory("thing", "plant", new ThingFactory<Plant>());
    installFactory("thing", "food", new ThingFactory<Food>());
    installFactory("thing", "stackable", new ThingFactory<Stackable>());
    installFactory("thing", "structure", new ThingFactory<Structure>());
}

Thing * EntityFactory::newThing(const std::string & type,
                                const Atlas::Message::Object & ent,
                                const edict_t & world)
{
    if (!ent.IsMap()) {
         debug( std::cout << "Entity is not a map" << std::endl << std::flush;);
    }
    Thing * thing = NULL;
    std::string py_package;
    fdict_t::iterator I = factories.find(type);
    if (type.size() == 0) {
        thing = new Thing();
    } else if (I != factories.end()) {
        thing = I->second->newThing();
    } else {
        fdict_t::const_iterator J;
        for(J = factories.begin(); J != factories.end(); J++) {
            const std::string & f_type = J->first;
            if (global_conf->findItem(f_type, type)) {
                thing = J->second->newThing();
                py_package = global_conf->getItem(f_type, type);
            }
        }
    }
    if (thing == NULL) {
        thing = new Thing();
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setType(type);
    // Sort out python object
    if (py_package.size() != 0) {
        Create_PyThing(thing, py_package, type);
    }
    const Atlas::Message::Object::MapType & entmap = ent.AsMap();
    Atlas::Message::Object::MapType::const_iterator K = entmap.find("name");
    if ((K != entmap.end()) && K->second.IsString()) {
        thing->setName(K->second.AsString());
    } else {
        debug( std::cout << "Got no name" << std::endl << std::flush;);
    }
    thing->merge(entmap);
    // Get location from entity, if it is present
    thing->getLocation(entmap, world);
    return thing;
}

void EntityFactory::flushFactories()
{
    fdict_t::const_iterator I = factories.begin();
    for (; I != factories.end(); I++) {
        delete I->second;
    }
}

void EntityFactory::installBaseClasses()
{
    global_conf->sigv.connect(SigC::slot(this, &EntityFactory::installClass));
}

void EntityFactory::installFactory(const std::string & parent,
                                   const std::string & className,
                                   FactoryBase * factory)
{
    if (factory != NULL) {
        factories[className] = factory;
    }

    Inheritance & i = Inheritance::instance();

    Root * r = new GameEntity();
    r->SetId(className);
    r->SetParents(Atlas::Message::Object::ListType(1, parent));
    i.addChild(r);

}

void EntityFactory::installClass(const std::string &parent,
                                 const std::string &className)
{
    if ((parent == "cyphesis") || (parent == "mind")) { return; }
    debug(std::cout << parent << ":" << className << std::endl << std::flush;);

    Inheritance & i = Inheritance::instance();
    
    Root * r = new GameEntity();
    r->SetId(className);
    r->SetParents(Atlas::Message::Object::ListType(1,parent));
    i.addChild(r);
}
