// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include <varconf/Config.h>

#include "EntityFactory.h"
#include "Persistance.h"

#include <rulesets/Thing.h>
#include <rulesets/Python_API.h>
#include <rulesets/ThingFactory.h>
#include <rulesets/Character.h>
#include <rulesets/Creator.h>
#include <rulesets/Plant.h>
#include <rulesets/Food.h>
#include <rulesets/Stackable.h>
#include <rulesets/Structure.h>

#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

static const bool debug_flag = false;

using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;

typedef Atlas::Message::Object MObject;

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
                                const MObject & ent,
                                const edict_t & world)
{
    if (!ent.IsMap()) {
         debug( std::cout << "Entity is not a map" << std::endl << std::flush;);
    }
    Thing * thing = NULL;
    MObject::MapType attributes;
    std::string py_package;
    fdict_t::iterator I = factories.find(type);
    if (type.size() == 0) {
        thing = new Thing();
    } else if (I != factories.end()) {
        FactoryBase * factory = I->second;
        thing = factory->newThing();
        attributes = factory->attributes;
        // Sort out python object
        if ((factory->language == "python") && (factory->script.size() != 0)) {
            std::cout << "Class " << type << " has a python class"
                      << std::endl << std::flush;
            Create_PyThing(thing, factory->script, type);
        }
    } else {
        std::cerr << "ERROR: Could not find a factory for " << type
                  << std::endl << std::flush;
    }
    if (thing == NULL) {
        thing = new Thing();
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setType(type);
    const MObject::MapType & entmap = ent.AsMap();
    // I am pretty sure this name handling is redundant
    MObject::MapType::const_iterator K = entmap.find("name");
    if ((K != entmap.end()) && K->second.IsString()) {
        thing->setName(K->second.AsString());
    } else {
        debug( std::cout << "Got no name" << std::endl << std::flush;);
    }
    for (K = entmap.begin(); K != entmap.end(); K++) {
        attributes[K->first] = K->second;
    }
#warning merge in default attributes from factory into entmap first.
    thing->merge(attributes);
    // Get location from entity, if it is present
    thing->getLocation(attributes, world);
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
    MObject::MapType ruleTable;
    Persistance * p = Persistance::instance();
    Inheritance & i = Inheritance::instance();
    p->getRules(ruleTable);

    MObject::MapType::const_iterator I = ruleTable.begin();
    for(; I != ruleTable.end(); ++I) {
        const MObject::MapType & classDesc = I->second.AsMap();
        MObject::MapType::const_iterator J = classDesc.find("parent");
        if ((J == classDesc.end()) || (!J->second.IsString())) { continue; }
        const std::string & parent = J->second.AsString();
        FactoryBase * f = getFactory(parent);
        J = classDesc.find("script");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            const MObject::MapType & script = J->second.AsMap();
            J = script.find("name");
            if ((J != script.end()) && (J->second.IsString())) {
                f->script = J->second.AsString();
                J = script.find("language");
                if ((J != script.end()) && (J->second.IsString())) {
                    f->language = J->second.AsString();
                }
            }
        }
        J = classDesc.find("attributes");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            f->attributes = J->second.AsMap();
        }
        installFactory(parent, I->first, f);
        std::cout << "INSTALLING " << I->first << ":" << parent << "{" << f->script << "." << f->language << "}" << std::endl << std::flush;
    }
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
    r->SetParents(MObject::ListType(1, parent));
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
    r->SetParents(MObject::ListType(1,parent));
    i.addChild(r);
}

FactoryBase * EntityFactory::getFactory(const std::string & parent)
{
    if (parent == "thing") { return new ThingFactory<Thing>(); }
    if (parent == "character") { return new ThingFactory<Character>(); }
    if (parent == "creator") { return new ThingFactory<Creator>(); }
    if (parent == "plant") { return new ThingFactory<Plant>(); }
    if (parent == "food") { return new ThingFactory<Food>(); }
    if (parent == "stackable") { return new ThingFactory<Stackable>(); }
    if (parent == "structure") { return new ThingFactory<Structure>(); }
    std::cerr << "NO FACTORY for " << parent << std::endl << std::flush;
    return new ThingFactory<Thing>();
}
