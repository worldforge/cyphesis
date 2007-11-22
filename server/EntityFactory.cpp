// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: EntityFactory.cpp,v 1.125 2007-11-22 00:47:39 alriddoch Exp $

#include <Python.h>

#include "EntityFactory.h"

#include "CorePropertyManager.h"
#include "PersistantThingFactory.h"
#include "ScriptFactory.h"
#include "TaskFactory.h"
#include "ArithmeticFactory.h"
#include "Persistance.h"
#include "Persistor.h"
#include "Player.h"

#include "rulesets/Thing.h"
#include "rulesets/MindFactory.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/Structure.h"
#include "rulesets/World.h"

#include "rulesets/Python_Script_Utils.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/random.h"
#include "common/compose.hpp"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/RootOperation.h>

#include <sys/types.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif // HAS_DIRENT_H


using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

static const bool debug_flag = false;

EntityFactory * EntityFactory::m_instance = NULL;

EntityFactory::EntityFactory(BaseWorld & w) : m_world(w)
{
    if (consts::enable_persistence && database_flag) {
        installFactory("world", "game_entity", new ForbiddenThingFactory<World>());
        PersistantThingFactory<Thing> * tft = new PersistantThingFactory<Thing>();
        installFactory("thing", "game_entity", tft);
        installFactory("character", "thing",
                       new PersistantThingFactory<Character>());
        installFactory("creator", "character",
                       new PersistantThingFactory<Creator>());
        installFactory("plant", "thing", new PersistantThingFactory<Plant>());
        installFactory("stackable","thing", 
                       new PersistantThingFactory<Stackable>());
        installFactory("structure", "thing",
                       new PersistantThingFactory<Structure>());
    } else {
        installFactory("world", "game_entity", new ThingFactory<World>());
        ThingFactory<Thing> * tft = new ThingFactory<Thing>();
        installFactory("thing", "game_entity", tft);
        installFactory("character", "thing", new ThingFactory<Character>());
        installFactory("creator", "character", new ThingFactory<Creator>());
        installFactory("plant", "thing", new ThingFactory<Plant>());
        installFactory("stackable", "thing", new ThingFactory<Stackable>());
        installFactory("structure", "thing", new ThingFactory<Structure>());
    }

    m_statisticsFactories["settler"] = new PythonArithmeticFactory("world.statistics.Statistics", "Statistics");

    // The property manager instance installs itself at construction time.
    new CorePropertyManager();
}

EntityFactory::~EntityFactory()
{
    delete PropertyManager::instance();
}

void EntityFactory::initWorld()
{
    if (!consts::enable_persistence) {
        return;
    }
    FactoryDict::const_iterator I = m_factories.find("world");
    if (I == m_factories.end()) {
        log(CRITICAL, "No world factory");
        return;
    }
    ForbiddenThingFactory<World> * wft = dynamic_cast<ForbiddenThingFactory<World> *>(I->second);
    if (wft == 0) {
        log(CRITICAL, "Its not a world factory");
        return;
    }
    wft->m_p.persist((World&)m_world.m_gameWorld);
}

Entity * EntityFactory::newEntity(const std::string & id, long intId,
                                  const std::string & type,
                                  const RootEntity & attributes) const
{
    debug(std::cout << "EntityFactor::newEntity()" << std::endl << std::flush;);
    Entity * thing = 0;
    FactoryDict::const_iterator I = m_factories.find(type);
    PersistorBase * pc = 0;
    if (I == m_factories.end()) {
        return 0;
    }
    FactoryBase * factory = I->second;
    thing = factory->newPersistantThing(id, intId, &pc);
    if (thing == 0) {
        return 0;
    }
    debug( std::cout << "[" << type << " " << thing->getName() << "]"
                     << std::endl << std::flush;);
    thing->setType(type);
    // Sort out python object
    if (factory->m_scriptFactory != 0) {
        debug(std::cout << "Class " << type << " has a python class"
                        << std::endl << std::flush;);
        factory->m_scriptFactory->addScript(thing);
    }
    //
    factory->populate(*thing);

    // Read the defaults
    thing->merge(factory->m_attributes);
    // And then override with the values provided for this entity.
    thing->merge(attributes->asMessage());
    // Get location from entity, if it is present
    // The default attributes cannot contain info on location
    if (attributes->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        const std::string & loc_id = attributes->getLoc();
        thing->m_location.m_loc = m_world.getEntity(loc_id);
    }
    if (thing->m_location.m_loc == 0) {
        // If no info was provided, put the entity in the game world
        thing->m_location.m_loc = &m_world.m_gameWorld;
    }
    thing->m_location.readFromEntity(attributes);
    if (!thing->m_location.pos().isValid()) {
        // If no position coords were provided, put it somewhere near origin
        thing->m_location.m_pos = Point3D(uniform(-8,8), uniform(-8,8), 0);
    }
    if (thing->m_location.velocity().isValid()) {
        if (attributes->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            log(ERROR, String::compose("EntityFactory::newEntity(%1, %2): "
                                       "Entity has velocity set from the "
                                       "attributes given by the creator",
                                       id, type));
        } else {
            log(ERROR, String::compose("EntityFactory::newEntity(%1, %2): "
                                       "Entity has velocity set from an "
                                       "unknown source", id, type));
        }
        thing->m_location.m_velocity.setValid(false);
    }
    if (pc != 0) {
        pc->persist();
        thing->clearUpdateFlags();
    }
    delete pc;
    return thing;
}

Task * EntityFactory::newTask(const std::string & name, Character & owner) const
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(name);
    if (I == m_taskFactories.end()) {
        return 0;
    }
    return I->second->newTask(owner);
}

Task * EntityFactory::activateTask(const std::string & tool,
                                   const std::string & op,
                                   const std::string & target,
                                   Character & owner) const
{
    TaskFactoryActivationDict::const_iterator I = m_taskActivations.find(tool);
    if (I == m_taskActivations.end()) {
        return 0;
    }
    const TaskFactoryMultimap & dict = I->second;
    TaskFactoryMultimap::const_iterator J = dict.lower_bound(op);
    if (J == dict.end()) {
        return 0;
    }
    TaskFactoryMultimap::const_iterator Jend = dict.upper_bound(op);
    for (; J != Jend; ++J) {
        if (!J->second->m_target.empty()) {
            if (!Inheritance::instance().isTypeOf(target, J->second->m_target)) {
                debug( std::cout << target << " is not a " << J->second->m_target
                                 << std::endl << std::flush; );
                continue;
            }
        }
        return J->second->newTask(owner);
    }
    return 0;
}

int EntityFactory::addStatisticsScript(Character & character) const
{
    StatisticsFactoryDict::const_iterator I = m_statisticsFactories.begin();
    if (I == m_statisticsFactories.end()) {
        return -1;
    }
    I->second->newScript(character);
    return 0;
}

void EntityFactory::flushFactories()
{
    FactoryDict::const_iterator Iend = m_factories.end();
    for (FactoryDict::const_iterator I = m_factories.begin(); I != Iend; ++I) {
        delete I->second;
    }
    m_factories.clear();
    StatisticsFactoryDict::const_iterator J = m_statisticsFactories.begin();
    StatisticsFactoryDict::const_iterator Jend = m_statisticsFactories.end();
    for (; J != Jend; ++J) {
        delete J->second;
    }
    m_statisticsFactories.clear();
    TaskFactoryDict::const_iterator K = m_taskFactories.begin();
    TaskFactoryDict::const_iterator Kend = m_taskFactories.end();
    for (; K != Kend; ++K) {
        delete K->second;
    }
    m_taskFactories.clear();
}

int EntityFactory::populateFactory(const std::string & class_name,
                                   FactoryBase * factory,
                                   const MapType & class_desc)
{
    // Establish whether this rule has an associated script, and
    // if so, use it.
    MapType::const_iterator J = class_desc.find("script");
    MapType::const_iterator Jend = class_desc.end();
    if (J != Jend && J->second.isMap()) {
        const MapType & script = J->second.asMap();
        J = script.find("name");
        if (J == script.end() || !J->second.isString()) {
            log(ERROR, String::compose("Entity \"%1\" script has no name.",
                                       class_name));
            return -1;
        }
        const std::string & script_name = J->second.String();
        J = script.find("language");
        if (J == script.end() || !J->second.isString()) {
            log(ERROR, String::compose("Entity \"%1\" script has no language.",
                                       class_name));
            return -1;
        }
        const std::string & script_language = J->second.String();
        if (script_language != "python") {
            log(ERROR, String::compose("Entity \"%1\" script has unknown "
                                       "language \"%2\".",
                                       class_name, script_language));
            return -1;
        }
        if (factory->m_scriptFactory != 0) {
            if (factory->m_scriptFactory->package() != script_name) {
                delete factory->m_scriptFactory;
                factory->m_scriptFactory = 0;
            }
        }
        if (factory->m_scriptFactory == 0) {
            factory->m_scriptFactory = new PythonScriptFactory(script_name,
                                                               class_name);
        }
    }

    // Establish whether this rule has an associated mind rule,
    // and handle it.
    J = class_desc.find("mind");
    if (J != Jend && J->second.isMap()) {
        const MapType & script = J->second.asMap();
        J = script.find("name");
        if (J != script.end() && J->second.isString()) {
            const std::string & mindType = J->second.String();
            // language is unused. might need it one day
            // J = script.find("language");
            // if (J != script.end() && J->second.isString()) {
                // const std::string & mindLang = J->second.String();
            // }
            MindFactory::instance()->addMindType(class_name, mindType);
        }
    }

    // Store the default attribute for entities create by this rule.
    J = class_desc.find("attributes");
    if (J != Jend && J->second.isMap()) {
        const MapType & attrs = J->second.asMap();
        MapType::const_iterator Kend = attrs.end();
        for (MapType::const_iterator K = attrs.begin(); K != Kend; ++K) {
            if (!K->second.isMap()) {
                log(ERROR, String::compose("Attribute description in rule %1 "
                                           "is not a map.", class_name));
                continue;
            }
            const MapType & attr = K->second.asMap();
            MapType::const_iterator L = attr.find("default");
            if (L != attr.end()) {
                // Store this value in the defaults for this class
                factory->m_classAttributes[K->first] = L->second;
                // and merge it with the defaults inherited from the parent
                factory->m_attributes[K->first] = L->second;
            }
        }
    }

    // Check whether it should be available to players as a playable character.
    J = class_desc.find("playable");
    if (J != Jend && J->second.isInt()) {
        Player::playableTypes.insert(class_name);
    }

    return 0;
}

bool EntityFactory::isTask(const std::string & class_name)
{
    if (class_name == "task") {
        return true;
    }
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

static void updateChildren(FactoryBase * factory)
{
    std::set<FactoryBase *>::const_iterator I = factory->m_children.begin();
    std::set<FactoryBase *>::const_iterator Iend = factory->m_children.end();
    for (; I != Iend; ++I) {
        FactoryBase * child_factory = *I;
        child_factory->m_attributes = factory->m_attributes;
        MapType::const_iterator J = child_factory->m_classAttributes.begin();
        MapType::const_iterator Jend = child_factory->m_classAttributes.end();
        for (; J != Jend; ++J) {
            child_factory->m_attributes[J->first] = J->second;
        }
        updateChildren(child_factory);
    }
}

int EntityFactory::installTaskClass(const std::string & class_name,
                                    const std::string & parent,
                                    const Root & class_desc)
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(class_name);
    if (I != m_taskFactories.end()) {
        log(ERROR, String::compose("Attempt to install task \"%1\" which is "
                                   "already installed.", class_name));
    }
    
    // Establish that this rule has an associated script.
    Element script_attr;
    if (class_desc->copyAttr("script", script_attr) != 0 ||
        !script_attr.isMap()) {
        log(ERROR, String::compose("Task \"%1\" has no script.", class_name));
        return -1;
    }
    const MapType & script = script_attr.Map();

    MapType::const_iterator J = script.find("name");
    MapType::const_iterator script_end = script.end();
    if (J == script_end || !J->second.isString()) {
        log(ERROR, String::compose("Task \"%1\" script has no name.",
                                   class_name));
        return -1;
    }
    const std::string & script_name = J->second.String();

    J = script.find("language");
    if (J == script_end || !J->second.isString()) {
        log(ERROR, String::compose("Task \"%1\" script has no language.",
                                   class_name));
        return -1;
    }
    const std::string & script_language = J->second.String();

    if (script_language != "python") {
        log(ERROR, String::compose("Task \"%1\" script has unknown language "
                                   "\"%2\".", class_name, script_language));
        return -1;
    }

    TaskFactory * factory = new PythonTaskScriptFactory(script_name, class_name);

    Element activation_attr;
    if (class_desc->copyAttr("activation", activation_attr) != 0 ||
        !activation_attr.isMap()) {
        delete factory;
        log(ERROR, String::compose("Task \"%1\" has no activation.",
                                   class_name));
        return -1;
    }
    const MapType & activation = activation_attr.Map();

    MapType::const_iterator act_end = activation.end();
    J = activation.find("tool");
    if (J == act_end || !J->second.isString()) {
        delete factory;
        log(ERROR, String::compose("Task \"%1\" activation has no tool.",
                                   class_name));
        return -1;
    }
    const std::string & activation_tool = J->second.String();

    Inheritance & i = Inheritance::instance();

    if (!i.hasClass(activation_tool)) {
        delete factory;
        waitForRule(class_name, class_desc, activation_tool,
                    String::compose("Task \"%1\" is activated by tool "
                                    "\"%2\" which does not exist.",
                                    class_name, activation_tool));
        return 1;
    }
    FactoryDict::const_iterator K = m_factories.find(activation_tool);
    if (K == m_factories.end()) {
        delete factory;
        log(ERROR, String::compose("Task class \"%1\" is activated "
                                   "by tool \"%2\" which is not an "
                                   "entity class.", class_name,
                                   activation_tool));
        return -1;
    }
    FactoryBase * tool_factory = K->second;

    J = activation.find("operation");
    if (J == act_end || !J->second.isString()) {
        delete factory;
        log(ERROR, String::compose("Task \"%1\" activation has no operation.",
                                   class_name));
        return -1;
    }

    const std::string & activation_op = J->second.String();
    if (!i.hasClass(activation_op)) {
        delete factory;
        waitForRule(class_name, class_desc, activation_op,
                    String::compose("Task \"%1\" is activated by operation "
                                    "\"%2\" which does not exist.",
                                    class_name, activation_op));
        return 1;
    }

    J = activation.find("target");
    if (J != act_end) {
        if (!J->second.isString()) {
            delete factory;
            log(ERROR, String::compose("Task \"%1\" activation has \"%2\" "
                                       " target.", class_name,
                                       Element::typeName(J->second.getType())));
            return -1;
        }
        const std::string & target_base = J->second.String();
        if (!i.hasClass(target_base)) {
            delete factory;
            waitForRule(class_name, class_desc, target_base,
                        String::compose("Task \"%1\" is activated on target "
                                        "\"%2\" which does not exist.",
                                        class_name, target_base));
            return 1;
        }
        factory->m_target = target_base;
    }

    m_taskActivations[activation_tool].insert(std::make_pair(activation_op, factory));
    MapType::iterator L = tool_factory->m_classAttributes.find("operations");
    if (L == tool_factory->m_classAttributes.end()) {
        tool_factory->m_classAttributes["operations"] = ListType(1, activation_op);
        tool_factory->m_attributes["operations"] = ListType(1, activation_op);
        updateChildren(tool_factory);
    } else {
        if (L->second.isList()) {
            ListType::const_iterator M = L->second.List().begin();
            for (; M != L->second.List().end() && *M != activation_op; ++M);
            if (M == L->second.List().end()) {
                L->second.List().push_back(activation_op);
                tool_factory->m_attributes[L->first] = L->second.List();
                updateChildren(tool_factory);
            }
        }
    }
    
    m_taskFactories.insert(std::make_pair(class_name, factory));

    i.addChild(class_desc);

    return 0;
}

int EntityFactory::installEntityClass(const std::string & class_name,
                                      const std::string & parent,
                                      const Root & class_desc)
{
    // Get the new factory for this rule
    FactoryDict::const_iterator I = m_factories.find(parent);
    if (I == m_factories.end()) {
        debug(std::cout << "class \"" << class_name
                        << "\" has non existant parent \"" << parent
                        << "\". Waiting." << std::endl << std::flush;);
        waitForRule(class_name, class_desc, parent,
                    String::compose("Entity rule \"%1\" has parent \"%2\" which"
                                    " does not exist.", class_name, parent));
        return 1;
    }
    FactoryBase * parent_factory = I->second;
    FactoryBase * factory = parent_factory->duplicateFactory();
    if (factory == 0) {
        log(ERROR,
            String::compose("Attempt to install rule \"%1\" which has parent "
                            "\"%2\" which cannot be instantiated",
                            class_name, parent));
        return -1;
    }

    assert(factory->m_parent == parent_factory);

    // Copy the defaults from the parent. In populateFactory this may be
    // overriden with the defaults for this class.
    factory->m_attributes = parent_factory->m_attributes;

    if (populateFactory(class_name, factory, class_desc->asMessage()) != 0) {
        delete factory;
        return -1;
    }

    debug(std::cout << "INSTALLING " << class_name << ":" << parent
                    << std::endl << std::flush;);

    // Install the factory in place.
    installFactory(class_name, parent, factory, class_desc);

    // Add it as a child to its parent.
    parent_factory->m_children.insert(factory);

    return 0;
}

int EntityFactory::installOpDefinition(const std::string & class_name,
                                       const std::string & parent,
                                       const Root & class_desc)
{
    Inheritance & i = Inheritance::instance();

    if (!i.hasClass(parent)) {
        debug(std::cout << "op_definition \"" << class_name
                        << "\" has non existant parent \"" << parent
                        << "\". Waiting." << std::endl << std::flush;);
        waitForRule(class_name, class_desc, parent,
                    String::compose("Operation \"%1\" has parent \"%2\" which "
                                    "does not exist.", class_name, parent));
        return 1;
    }

    Atlas::Objects::Root r = atlasOpDefinition(class_name, parent);

    if (i.addChild(class_desc) != 0) {
        return -1;
    }

    int op_no = Atlas::Objects::Factories::instance()->addFactory(class_name, &Atlas::Objects::generic_factory);
    i.opInstall(class_name, op_no);

    return 0;
}

int EntityFactory::installRule(const std::string & class_name,
                               const Root & class_desc)
{
    const std::string & objtype = class_desc->getObjtype();
    const std::list<std::string> & parents = class_desc->getParents();
    if (parents.empty()) {
        log(ERROR, String::compose("Rule \"%1\" has empty parents. Skipping.",
                                   class_name));
        return -1;
    }
    const std::string & parent = parents.front();
    if (parent.empty()) {
        log(ERROR, String::compose("Rule \"%1\" has empty first parent."
                                   " Skipping.", class_name));
        return -1;
    }
    if (objtype == "class") {
        if (isTask(parent)) {
            int ret = installTaskClass(class_name, parent, class_desc);
            if (ret != 0) {
                return ret;
            }
        } else {
            int ret = installEntityClass(class_name, parent, class_desc);
            if (ret != 0) {
                return ret;
            }
        }
    } else if (objtype == "op_definition") {
        int ret = installOpDefinition(class_name, parent, class_desc);
        if (ret != 0) {
            return ret;
        }
    } else {
        log(ERROR, String::compose("Rule \"%1\" has unknown objtype=\"%2\". "
                                   "Skipping.", class_name, objtype));
        return -1;
    }

    // Install any rules that were waiting for this rule before they
    // could be installed
    RuleWaitList::iterator I = m_waitingRules.lower_bound(class_name);
    RuleWaitList::iterator Iend = m_waitingRules.upper_bound(class_name);
    std::map<std::string, Root> readyRules;
    for (; I != Iend; ++I) {
        const std::string & wClassName = I->second.name;
        const Root & wClassDesc = I->second.desc;
        readyRules.insert(std::make_pair(wClassName, wClassDesc));
        debug(std::cout << "WAITING rule " << wClassName
                        << " now ready from " << class_name
                        << std::endl << std::flush;);
    }
    m_waitingRules.erase(class_name);
        
    std::map<std::string, Root>::const_iterator K = readyRules.begin();
    std::map<std::string, Root>::const_iterator Kend = readyRules.end();
    for (; K != Kend; ++K) {
        const std::string & rClassName = K->first;
        const Root & rClassDesc = K->second;
        installRule(rClassName, rClassDesc);
    }
    return 0;
}

int EntityFactory::modifyEntityClass(const std::string & class_name,
                                     const Root & class_desc)
{
    FactoryDict::const_iterator I = m_factories.find(class_name);
    if (I == m_factories.end()) {
        log(ERROR, String::compose("Could not find factory for existing "
                                   "entity class \"%1\".", class_name));
        return -1;
    }
    FactoryBase * factory = I->second;
    assert(factory != 0);
    
    ScriptFactory * script_factory = factory->m_scriptFactory;
    if (script_factory != 0) {
        script_factory->refreshClass();
    }

    MapType backup_attributes = factory->m_attributes,
            backup_class_attributes = factory->m_classAttributes;

    // Copy the defaults from the parent. In populateFactory this may be
    // overriden with the defaults for this class.
    if (factory->m_parent != 0) {
        factory->m_attributes = factory->m_parent->m_attributes;
    } else {
        // This is non fatal, but nice to know it has happened.
        // This should only happen if the client attempted to modify the
        // type data for a core hard coded type.
        log(ERROR, String::compose("EntityFactory::modifyEntityClass: \"%1\" "
                                   "modified by client, but has no parent "
                                   "factory.", class_name));
        factory->m_attributes = MapType();
    }
    factory->m_classAttributes = MapType();

    if (populateFactory(class_name, factory, class_desc->asMessage()) != 0) {
        factory->m_attributes = backup_attributes;
        factory->m_classAttributes = backup_class_attributes;
        return -1;
    }

    updateChildren(factory);

    return 0;
}

int EntityFactory::modifyTaskClass(const std::string & class_name,
                                   const Root & class_desc)
{
    TaskFactoryDict::const_iterator I = m_taskFactories.find(class_name);
    if (I == m_taskFactories.end()) {
        log(ERROR, String::compose("Could not find factory for existing task "
                                   "class \"%1\"", class_name));
        return -1;
    }
    // FIXME Actually update the task factory.
    TaskFactory * factory = I->second;
    assert(factory != 0);

    return 0;
}

int EntityFactory::modifyOpDefinition(const std::string & class_name,
                                      const Root & class_desc)
{
    // Nothing to actually do
    return 0;
}

int EntityFactory::modifyRule(const std::string & class_name,
                              const Root & class_desc)
{
    Root o = Inheritance::instance().getClass(class_name);
    if (!o.isValid()) {
        log(ERROR, String::compose("Could not find existing type \"%1\" "
                                   "in inheritance", class_name));
        return -1;
    }
    if (o->getParents().front() == "task") {
        return modifyTaskClass(class_name, class_desc);
    } else if (class_desc->getObjtype() == "op_definition") {
        return modifyOpDefinition(class_name, class_desc);
    } else {
        return modifyEntityClass(class_name, class_desc);
    }
}

/// \brief Mark a rule down as waiting for another.
///
/// Note that a rule cannot yet be installed because it depends on something
/// that has not yet occured, or a more fatal condition has occured.
void EntityFactory::waitForRule(const std::string & rulename,
                                const Root & ruledesc,
                                const std::string & dependent,
                                const std::string & reason)
{
    RuleWaiting rule;
    rule.name = rulename;
    rule.desc = ruledesc;
    rule.reason = reason;

    m_waitingRules.insert(std::make_pair(dependent, rule));
}

void EntityFactory::getRulesFromFiles(std::map<std::string, Root> & rules)
{
    std::string filename;

    std::string dirname = etc_directory + "/cyphesis/" + ruleset + ".d";
    DIR * rules_dir = ::opendir(dirname.c_str());
    if (rules_dir == 0) {
        filename = etc_directory + "/cyphesis/" + ruleset + ".xml";
        AtlasFileLoader f(filename, rules);
        if (f.isOpen()) {
            log(WARNING, String::compose("Reading legacy rule data from \"%1\".",
                                         filename));
            f.read();
        }
        return;
    }
    while (struct dirent * rules_entry = ::readdir(rules_dir)) {
        if (rules_entry->d_name[0] == '.') {
            continue;
        }
        filename = dirname + "/" + rules_entry->d_name;
        
        AtlasFileLoader f(filename, rules);
        if (!f.isOpen()) {
            log(ERROR, String::compose("Unable to open rule file \"%1\".",
                                       filename));
        } else {
            f.read();
        }
    }
    ::closedir(rules_dir);
}

void EntityFactory::installRules()
{
    std::map<std::string, Root> ruleTable;

    if (database_flag) {
        // FIXME Persistance * p = Persistance::instance();
        // FIXME p->getRules(ruleTable);
    } else {
        getRulesFromFiles(ruleTable);
    }

    if (ruleTable.empty()) {
        log(ERROR, "Rule database table contains no rules.");
        if (database_flag) {
            log(NOTICE, "Attempting to load temporary ruleset from files.");
            getRulesFromFiles(ruleTable);
        }
    }

    RootDict::const_iterator Iend = ruleTable.end();
    for (RootDict::const_iterator I = ruleTable.begin(); I != Iend; ++I) {
        const std::string & class_name = I->first;
        const Root & class_desc = I->second;
        installRule(class_name, class_desc);
    }
    // Report on the non-cleared rules.
    // Perhaps we can keep them too?
    // m_waitingRules.clear();
    RuleWaitList::const_iterator J = m_waitingRules.begin();
    RuleWaitList::const_iterator Jend = m_waitingRules.end();
    for (; J != Jend; ++J) {
        log(ERROR, J->second.reason);
    }
}

void EntityFactory::installFactory(const std::string & class_name,
                                   const std::string & parent,
                                   FactoryBase * factory,
                                   Root class_desc)
{
    assert(factory != 0);

    m_factories[class_name] = factory;

    Inheritance & i = Inheritance::instance();

    if (class_desc.isValid()) {
        assert(class_desc->getId() == class_name);
        assert(class_desc->getParents().front() == parent);
        i.addChild(class_desc);
    } else {
        i.addChild(atlasClass(class_name, parent));
    }
}

FactoryBase * EntityFactory::getNewFactory(const std::string & parent)
{
    FactoryDict::const_iterator I = m_factories.find(parent);
    if (I == m_factories.end()) {
        return 0;
    }
    return I->second->duplicateFactory();
}
