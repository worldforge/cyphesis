// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include "EntityFactory.h"
#include "Persistance.h"

#include <rulesets/Thing.h>
#include <rulesets/Python_API.h>
#include <rulesets/MindFactory.h>
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
                                const MObject::MapType & entmap,
                                const edict_t & world)
{
    Thing * thing = NULL;
    MObject::MapType attributes;
    fdict_t::iterator I = factories.find(type);
    if (I != factories.end()) {
        FactoryBase * factory = I->second;
        thing = factory->newThing();
        attributes = factory->attributes;
        // Sort out python object
        if ((factory->language == "python") && (factory->script.size() != 0)) {
            debug(std::cout << "Class " << type << " has a python class"
                            << std::endl << std::flush;);
            Create_PyThing(thing, factory->script, type);
        }
    } else {
        if (type.size() == 0) {
            std::cerr << "NOTICE: Empty string type passed to EntityFactory::newThing" << std::endl << std::flush;
        } else {
            installFactory("thing", type, new ThingFactory<Thing>());
            std::cerr << "NOTICE: Installing patch-in factory for " << type
                      << std::endl << std::flush;
        }
        thing = new Thing();
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setType(type);
    // I am pretty sure this name handling is redundant
    MObject::MapType::const_iterator K = entmap.find("name");
    if ((K != entmap.end()) && K->second.IsString()) {
        thing->setName(K->second.AsString());
    } else {
        debug( std::cout << "Got no name" << std::endl << std::flush;);
    }
    // merge attributes from the creation op over default attributes.
    for (K = entmap.begin(); K != entmap.end(); K++) {
        attributes[K->first] = K->second;
    }
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
    p->getRules(ruleTable);

    MObject::MapType::const_iterator I = ruleTable.begin();
    for(; I != ruleTable.end(); ++I) {
        const std::string & type = I->first;
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
        J = classDesc.find("mind");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            const MObject::MapType & script = J->second.AsMap();
            J = script.find("name");
            if ((J != script.end()) && (J->second.IsString())) {
                const std::string mindType = J->second.AsString();
                // language is unused. might need it one day
                // J = script.find("language");
                // if ((J != script.end()) && (J->second.IsString())) {
                    // const std::string mindLang = J->second.AsString();
                // }
                MindFactory::instance()->addMindType(type, mindType);
            }
        }
        J = classDesc.find("attributes");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            f->attributes = J->second.AsMap();
        }
        installFactory(parent, type, f);
        debug(std::cout << "INSTALLING " << type << ":" << parent
                        << "{" << f->script << "." << f->language << "}"
                        << std::endl << std::flush;);
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
