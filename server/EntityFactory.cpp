// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "EntityFactory.h"

#include "PersistantThingFactory.h"
#include "Persistance.h"
#include "Persistor.h"
#include "Player.h"

#include <rulesets/Thing.h>
#include <rulesets/MindFactory.h>
#include <rulesets/Character.h>
#include <rulesets/Creator.h>
#include <rulesets/Plant.h>
#include <rulesets/Food.h>
#include <rulesets/Stackable.h>
#include <rulesets/Structure.h>
#include <rulesets/Line.h>
#include <rulesets/Area.h>
#include <rulesets/World.h>

#include <rulesets/Python_API.h>

#include <common/debug.h>
#include <common/globals.h>
#include <common/inheritance.h>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Entity/GameEntity.h>

static const bool debug_flag = false;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory(BaseWorld & w) : m_world(w),
             m_eft(new PersistantThingFactory<Entity>())
{
    // This class can only have one instance, so a Factory is not installed
    // FIXME Add a factory in here so we have persist the world properlly

    PersistantThingFactory<World> * wft = new PersistantThingFactory<World>();
    installFactory("game_entity", "world", wft);
    wft->m_p.persist((World &)m_world.gameWorld);

    PersistantThingFactory<Thing> * tft = new PersistantThingFactory<Thing>();
    installFactory("game_entity", "thing", tft);
    installFactory("thing", "feature", tft->duplicateFactory());
    installFactory("feature", "line", new PersistantThingFactory<Line>());
    installFactory("feature", "area", new PersistantThingFactory<Area>());
    installFactory("thing", "character",
                   new PersistantThingFactory<Character>());
    installFactory("character", "creator",
                   new PersistantThingFactory<Creator>());
    installFactory("thing", "plant", new PersistantThingFactory<Plant>());
    installFactory("thing", "food", new PersistantThingFactory<Food>());
    installFactory("thing", "stackable",
                   new PersistantThingFactory<Stackable>());
    installFactory("thing", "structure",
                   new PersistantThingFactory<Structure>());
}

Entity * EntityFactory::newEntity(const std::string & id,
                                  const std::string & type,
                                  const Fragment::MapType & entmap)
{
    // FIXME Re-write to use persistant stuff
    Entity * thing = 0;
    Fragment::MapType attributes;
    FactoryDict::const_iterator I = factories.find(type);
    PersistorBase * pc = 0;
    if (I != factories.end()) {
        FactoryBase * factory = I->second;
        thing = factory->newPersistantThing(&pc);
        attributes = factory->attributes;
        // Sort out python object
        if ((factory->language == "python") && (!factory->script.empty())) {
            debug(std::cout << "Class " << type << " has a python class"
                            << std::endl << std::flush;);
            Create_PyThing(thing, factory->script, type);
        }
    } else {
        // This should be tolerated less
        if (type.empty()) {
            log(ERROR, "Empty type passed to EntityFactory::newEntity");
        } else {
            std::string msg = std::string("Unknown type ") + type + " passed to EntityFactory::newEntity";
            log(ERROR, msg.c_str());
        }
        thing = new Thing();
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setId(id);
    thing->setType(type);
    // merge attributes from the creation op over default attributes.
    // FIXME Is it practical to avoid this merge copy by calling merge twice?
    // Might cause a problem with getLocation
    Fragment::MapType::const_iterator K = entmap.begin();
    for (; K != entmap.end(); K++) {
        attributes[K->first] = K->second;
    }
    thing->merge(attributes);
    // Get location from entity, if it is present
    if (thing->getLocation(attributes, m_world.getObjects())) {
        thing->location.ref = &m_world.gameWorld;
    }
    if (!thing->location.coords.isValid()) {
        thing->location.coords = Vector3D(0,0,0);
    }
    if (pc != 0) {
        pc->persist();
        thing->clearUpdateFlags();
    }
    delete pc;
    return thing;
}

void EntityFactory::flushFactories()
{
    FactoryDict::const_iterator I = factories.begin();
    for (; I != factories.end(); I++) {
        delete I->second;
    }
    delete m_eft;
    m_eft = NULL;
}

void EntityFactory::installBaseClasses()
{
    Fragment::MapType ruleTable;
    Persistance * p = Persistance::instance();
    p->getRules(ruleTable);

    Fragment::MapType::const_iterator I = ruleTable.begin();
    for(; I != ruleTable.end(); ++I) {
        const std::string & type = I->first;
        const Fragment::MapType & classDesc = I->second.AsMap();
        Fragment::MapType::const_iterator J = classDesc.find("parent");
        if ((J == classDesc.end()) || (!J->second.IsString())) { continue; }
        const std::string & parent = J->second.AsString();
        FactoryBase * f = getFactory(parent);
        J = classDesc.find("script");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            const Fragment::MapType & script = J->second.AsMap();
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
            const Fragment::MapType & script = J->second.AsMap();
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
        J = classDesc.find("playable");
        if ((J != classDesc.end()) && (J->second.IsInt())) {
            Player::playableTypes.insert(type);
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

    Atlas::Objects::Root * r = new Atlas::Objects::Entity::GameEntity();
    r->SetId(className);
    r->SetParents(Fragment::ListType(1, parent));
    i.addChild(r);

}

FactoryBase * EntityFactory::getFactory(const std::string & parent)
{
    FactoryDict::const_iterator I = factories.find(parent);
    if (I == factories.end()) {
        std::string msg = std::string("Failed to find factory for ") + parent
                     + " while installing a new type which inherits from it.";
        log(WARNING, msg.c_str());
        // FIXME Don't creat another FSCKing factory here
        return new PersistantThingFactory<Thing>();
    }
    return I->second->duplicateFactory();
}
