// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "EntityFactory.h"

#include "PersistantThingFactory.h"
#include "Persistance.h"
#include "Persistor.h"
#include "Player.h"

#include "rulesets/Thing.h"
#include "rulesets/MindFactory.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Food.h"
#include "rulesets/Stackable.h"
#include "rulesets/Structure.h"
#include "rulesets/Line.h"
#include "rulesets/Area.h"
#include "rulesets/World.h"

#include "rulesets/Python_API.h"

#include "common/debug.h"
#include "common/globals.h"
#include "common/inheritance.h"

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Entity/GameEntity.h>

static const bool debug_flag = false;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory(BaseWorld & w) : m_world(w),
             m_eft(new PersistantThingFactory<Entity>())
{
    // This class can only have one instance, so a Factory is not installed
    PersistantThingFactory<World> * wft = new PersistantThingFactory<World>();
    installFactory("game_entity", "world", wft);
    wft->m_p.persist((World &)m_world.m_gameWorld);

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
                                  const Element::MapType & entmap)
{
    debug(std::cout << "EntityFactor::newEntity()" << std::endl << std::flush;);
    Entity * thing = 0;
    Element::MapType attributes;
    FactoryDict::const_iterator I = m_factories.find(type);
    PersistorBase * pc = 0;
    if (I != m_factories.end()) {
        FactoryBase * factory = I->second;
        thing = factory->newPersistantThing(id, &pc);
        // FIXME Avoid this copy
        attributes = factory->m_attributes;
        // Sort out python object
        if ((factory->m_language == "python") && (!factory->m_script.empty())) {
            debug(std::cout << "Class " << type << " has a python class"
                            << std::endl << std::flush;);
            Create_PyEntity(thing, factory->m_script, type);
        }
    } else {
        // This should be tolerated less
        if (type.empty()) {
            log(ERROR, "Empty type passed to EntityFactory::newEntity");
        } else {
            std::string msg = std::string("Unknown type ") + type + " passed to EntityFactory::newEntity";
            log(ERROR, msg.c_str());
        }
        thing = new Thing(id);
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    // thing->setId(id);
    thing->setType(type);
    // merge attributes from the creation op over default attributes.
    // FIXME Is it practical to avoid this merge copy by calling merge twice?
    // Might cause a problem with getLocation
    Element::MapType::const_iterator K = entmap.begin();
    for (; K != entmap.end(); K++) {
        attributes[K->first] = K->second;
    }
    thing->merge(attributes);
    // Get location from entity, if it is present
    if (thing->getLocation(attributes, m_world.getObjects())) {
        thing->m_location.m_loc = &m_world.m_gameWorld;
    }
    if (!thing->m_location.m_pos.isValid()) {
        thing->m_location.m_pos = Vector3D(0,0,0);
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
    FactoryDict::const_iterator I = m_factories.begin();
    for (; I != m_factories.end(); I++) {
        delete I->second;
    }
    delete m_eft;
    m_eft = NULL;
}

void EntityFactory::installBaseClasses()
{
    Element::MapType ruleTable;
    Persistance * p = Persistance::instance();
    p->getRules(ruleTable);

    Element::MapType::const_iterator I = ruleTable.begin();
    for(; I != ruleTable.end(); ++I) {
        const std::string & type = I->first;
        const Element::MapType & classDesc = I->second.AsMap();
        Element::MapType::const_iterator J = classDesc.find("parent");
        if ((J == classDesc.end()) || (!J->second.IsString())) { continue; }
        const std::string & parent = J->second.AsString();
        FactoryBase * f = getFactory(parent);
        J = classDesc.find("script");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            const Element::MapType & script = J->second.AsMap();
            J = script.find("name");
            if ((J != script.end()) && (J->second.IsString())) {
                f->m_script = J->second.AsString();
                J = script.find("language");
                if ((J != script.end()) && (J->second.IsString())) {
                    f->m_language = J->second.AsString();
                }
            }
        }
        J = classDesc.find("mind");
        if ((J != classDesc.end()) && (J->second.IsMap())) {
            const Element::MapType & script = J->second.AsMap();
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
            f->m_attributes = J->second.AsMap();
        }
        J = classDesc.find("playable");
        if ((J != classDesc.end()) && (J->second.IsInt())) {
            Player::playableTypes.insert(type);
        }
        installFactory(parent, type, f);
        debug(std::cout << "INSTALLING " << type << ":" << parent
                        << "{" << f->m_script << "." << f->m_language << "}"
                        << std::endl << std::flush;);
    }
}

void EntityFactory::installFactory(const std::string & parent,
                                   const std::string & className,
                                   FactoryBase * factory)
{
    if (factory != NULL) {
        m_factories[className] = factory;
    }

    Inheritance & i = Inheritance::instance();

    Atlas::Objects::Root * r = new Atlas::Objects::Entity::GameEntity();
    r->SetId(className);
    r->SetParents(Element::ListType(1, parent));
    i.addChild(r);

}

FactoryBase * EntityFactory::getFactory(const std::string & parent)
{
    FactoryDict::const_iterator I = m_factories.find(parent);
    if (I == m_factories.end()) {
        std::string msg = std::string("Failed to find factory for ") + parent
                     + " while installing a new type which inherits from it.";
        log(WARNING, msg.c_str());
        // FIXME Don't creat another FSCKing factory here
        return new PersistantThingFactory<Thing>();
    }
    return I->second->duplicateFactory();
}
