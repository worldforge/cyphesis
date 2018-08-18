// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#include "EntityRuleHandler.h"

#include "EntityBuilder.h"
#include "EntityFactory.h"
#include "Player.h"

#include "rulesets/PythonScriptFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/Inheritance.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;



class Entity;

class Stackable;

class Thing;

class World;

extern template
class EntityFactory<Thing>;

extern template
class EntityFactory<Stackable>;

extern template
class EntityFactory<World>;

static const bool debug_flag = false;


EntityRuleHandler::EntityRuleHandler(EntityBuilder * eb)
    : m_builder(eb)
{

    mFactories["world"] = [](EntityFactoryBase* parent) -> EntityFactoryBase* {
        auto factory = new EntityFactory<World>();
        factory->m_parent = parent;
        return factory;
    };
    mFactories["thing"] = [](EntityFactoryBase* parent) -> EntityFactoryBase* {
        auto factory = new EntityFactory<Thing>();
        factory->m_parent = parent;
        return factory;
    };
    mFactories["stackable"] = [](EntityFactoryBase* parent) -> EntityFactoryBase* {
        auto factory = new EntityFactory<Stackable>();
        factory->m_parent = parent;
        return factory;
    };

}


int EntityRuleHandler::installEntityClass(const std::string & class_name,
                                          const std::string & parent,
                                          const Root & class_desc,
                                          std::string & dependent,
                                          std::string & reason,
                                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    EntityFactoryBase* factory;
    if (parent == "game_entity") {
        auto I = mFactories.find(class_name);
        if (I != mFactories.end()) {
            factory = I->second(nullptr);
        } else {
            debug(std::cout << "class \"" << class_name
                            << "\" has non existent parent \"" << parent
                            << "\". Waiting." << std::endl << std::flush;);
            dependent = parent;
            reason = compose("Entity rule \"%1\" has parent 'game_entity' and requires a "
                             "pre-defined entity factory, which could not be found.", class_name);
            return 1;
        }
    } else {
        auto parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(parent));
        // Get the new factory for this rule
        if (parent_factory == nullptr) {
            debug(std::cout << "class \"" << class_name
                            << "\" has non existent parent \"" << parent
                            << "\". Waiting." << std::endl << std::flush;);
            dependent = parent;
            reason = compose("Entity rule \"%1\" has parent \"%2\" which does "
                             "not exist.", class_name, parent);
            return 1;
        }

        auto I = mFactories.find(class_name);
        if (I != mFactories.end()) {
            factory = I->second(parent_factory);
        } else {
            factory = parent_factory->duplicateFactory();
        }
        assert(factory->m_parent == parent_factory);
    }

    return installEntityClass(class_name, parent, class_desc, dependent, reason, factory, changes);

}

int EntityRuleHandler::installEntityClass(const std::string & class_name,
                                          const std::string & parent,
                                          const Root & class_desc,
                                          std::string & dependent,
                                          std::string & reason,
                                          EntityFactoryBase* factory,
                                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    // Get the new factory for this rule

    if (factory == nullptr) {
        log(ERROR,
            compose("Attempt to install rule \"%1\" which has parent \"%2\" "
                        "which cannot be instantiated", class_name, parent));
        return -1;
    }

    if (populateEntityFactory(class_name, factory,
                              class_desc->asMessage(),
                              dependent,
                              reason) != 0) {
        delete factory;
        return -1;
    }

    debug(std::cout << "INSTALLING " << class_name << ":" << parent
                    << std::endl << std::flush;);

    // Install the factory in place.
    if (m_builder->installFactory(class_name, class_desc, factory) != 0) {
        delete factory;
        return -1;
    }

    factory->addProperties();

    auto parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(parent));
    if (parent_factory) {
        // Add it as a child to its parent.
        parent_factory->m_children.insert(factory);
        parent_factory->updateProperties(changes);
    }

    return 0;
}
int EntityRuleHandler::modifyEntityClass(const std::string & class_name,
                                         const Root & class_desc,
                                         std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    auto factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(class_name));
    if (factory == nullptr) {
        log(ERROR, compose("Could not find factory for existing entity class "
                           "\"%1\".", class_name));
        return -1;
    }
    assert(factory != nullptr);
    
    MapType backup_attributes = factory->m_attributes,
            backup_class_attributes = factory->m_classAttributes;

    // Copy the defaults from the parent. In populateEntityFactory this may be
    // overridden with the defaults for this class.
    if (factory->m_parent != nullptr) {
        factory->m_attributes = factory->m_parent->m_attributes;
    } else {
        // This is non fatal, but nice to know it has happened.
        // This should only happen if the client attempted to modify the
        // type data for a core hard coded type.
        log(ERROR, compose("EntityRuleHandler::modifyEntityClass: \"%1\" modified "
                           "by client, but has no parent factory.",
                           class_name));
        factory->m_attributes = MapType();
    }
    factory->m_classAttributes = MapType();

    std::string dependent, reason;
    if (populateEntityFactory(class_name, factory,
                              class_desc->asMessage(),
                               dependent, reason) != 0) {
        factory->m_attributes = backup_attributes;
        factory->m_classAttributes = backup_class_attributes;
        return -1;
    }

    factory->updateProperties(changes);

    return 0;
}


int EntityRuleHandler::populateEntityFactory(const std::string & class_name,
                                             EntityFactoryBase * factory,
                                             const MapType & class_desc,
                                             std::string & dependent,
                                             std::string & reason)
{
    // assert(class_name == class_desc->getId());

    // Store the default attribute for entities create by this rule.
    auto J = class_desc.find("attributes");
    if (J != class_desc.end() && J->second.isMap()) {
        const MapType & attrs = J->second.asMap();
        auto Kend = attrs.end();
        for (auto K = attrs.begin(); K != Kend; ++K) {
            if (!K->second.isMap()) {
                log(ERROR, compose("Attribute description in rule %1 is not a "
                                   "map.", class_name));
                continue;
            }
            const MapType & attr = K->second.asMap();
            Atlas::Message::Element defaultValue;
            auto L = attr.find("default");
            if (L != attr.end()) {
                defaultValue = L->second;
            }
            // Store this value in the defaults for this class
            factory->m_classAttributes[K->first] = defaultValue;
            // and merge it with the defaults inherited from the parent
            factory->m_attributes[K->first] = defaultValue;
        }
    }

    return 0;
}

int EntityRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(!desc->getParent().empty());
    if (desc->getObjtype() == "class") {
        return 0;
    }
    return -1;
}

int EntityRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason,
                             std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return installEntityClass(name, parent, description, dependent, reason, changes);
}

int EntityRuleHandler::update(const std::string & name,
                                const Atlas::Objects::Root & desc,
                                std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return modifyEntityClass(name, desc, changes);
}
