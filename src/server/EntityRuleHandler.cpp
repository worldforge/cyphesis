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

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;


class Entity;

class Thing;

class World;

extern template
class EntityFactory<Thing>;

extern template
class EntityFactory<World>;

static const bool debug_flag = false;


EntityRuleHandler::EntityRuleHandler(EntityBuilder& eb, const PropertyManager& propertyManager)
        : m_builder(eb), m_propertyManager(propertyManager)
{

    mFactories["world"] = [](EntityFactoryBase* parent) -> std::unique_ptr<EntityFactoryBase> {
        auto factory = std::make_unique<EntityFactory<World>>();
        factory->m_parent = parent;
        return factory;
    };
    mFactories["thing"] = [](EntityFactoryBase* parent) -> std::unique_ptr<EntityFactoryBase> {
        auto factory = std::make_unique<EntityFactory<Thing>>();
        factory->m_parent = parent;
        return factory;
    };

}


int EntityRuleHandler::installEntityClass(const std::string& class_name,
                                          const std::string& parent,
                                          const Root& class_desc,
                                          std::string& dependent,
                                          std::string& reason,
                                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    std::unique_ptr<EntityFactoryBase> factory;
    if (parent == "game_entity") {
        auto I = mFactories.find(class_name);
        if (I != mFactories.end()) {
            factory = I->second(nullptr);
        } else {
            debug_print("class \"" << class_name
                            << "\" has non existent parent \"" << parent
                            << "\". Waiting.")
            dependent = parent;
            reason = compose("Entity rule \"%1\" has parent 'game_entity' and requires a "
                             "pre-defined entity factory, which could not be found.", class_name);
            return 1;
        }
    } else {
        auto parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder.getClassFactory(parent));
        // Get the new factory for this rule
        if (parent_factory == nullptr) {
            debug_print("class \"" << class_name
                            << "\" has non existent parent \"" << parent
                            << "\". Waiting.")
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

    return installEntityClass(class_name, parent, class_desc, dependent, reason, std::move(factory), changes);

}

int EntityRuleHandler::installEntityClass(const std::string& class_name,
                                          const std::string& parent,
                                          const Root& class_desc,
                                          std::string& dependent,
                                          std::string& reason,
                                          std::unique_ptr<EntityFactoryBase> factory,
                                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    // Get the new factory for this rule

    if (factory == nullptr) {
        log(ERROR,
            compose("Attempt to install rule \"%1\" which has parent \"%2\" "
                    "which cannot be instantiated", class_name, parent));
        return -1;
    }

    if (populateEntityFactory(class_name, factory.get(),
                              class_desc->asMessage(),
                              dependent,
                              reason) != 0) {
        return -1;
    }

    debug_print("INSTALLING " << class_name << ":" << parent)

    auto factoryPtr = factory.get();
    // Install the factory in place.
    if (m_builder.installFactory(class_name, class_desc, std::move(factory)) != 0) {
        return -1;
    }

    factoryPtr->addProperties(m_propertyManager);

    auto parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder.getClassFactory(parent));
    if (parent_factory) {
        // Add it as a child to its parent.
        parent_factory->m_children.insert(factoryPtr);
        parent_factory->updateProperties(changes, m_propertyManager);
    }

    return 0;
}

int EntityRuleHandler::modifyEntityClass(const std::string& class_name,
                                         const Root& class_desc,
                                         std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    auto factory = dynamic_cast<EntityFactoryBase*>(m_builder.getClassFactory(class_name));
    if (factory == nullptr) {
        log(ERROR, compose("Could not find factory for existing entity class "
                           "\"%1\".", class_name));
        return -1;
    }
    assert(factory != nullptr);

    MapType backup_attributes = factory->m_attributes;
    auto backup_class_attributes = factory->m_classAttributes;

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
    factory->m_classAttributes.clear();

    std::string dependent, reason;
    if (populateEntityFactory(class_name, factory,
                              class_desc->asMessage(),
                              dependent, reason) != 0) {
        factory->m_attributes = backup_attributes;
        factory->m_classAttributes = backup_class_attributes;
        return -1;
    }

    factory->updateProperties(changes, m_propertyManager);

    return 0;
}


int EntityRuleHandler::populateEntityFactory(const std::string& class_name,
                                             EntityFactoryBase* factory,
                                             const MapType& class_desc,
                                             std::string& dependent,
                                             std::string& reason)
{
    // assert(class_name == class_desc->getId());

    // Store the default attribute for entities create by this rule.
    auto J = class_desc.find("attributes");
    if (J != class_desc.end() && J->second.isMap()) {
        const MapType& attrs = J->second.asMap();
        for (const auto& entry : attrs) {
            if (!entry.second.isMap()) {
                log(ERROR, compose("Attribute '%1' in rule '%2' is not a "
                                   "map.", entry.first, class_name));
                continue;
            }
            const MapType& attr = entry.second.asMap();
            ClassAttribute classAttribute;
            auto L = attr.find("default");
            if (L != attr.end()) {
                classAttribute.defaultValue = L->second;
            }
            L = attr.find("append");
            if (L != attr.end()) {
                classAttribute.append = L->second;
            }
            L = attr.find("prepend");
            if (L != attr.end()) {
                classAttribute.prepend = L->second;
            }
            L = attr.find("subtract");
            if (L != attr.end()) {
                classAttribute.subtract = L->second;
            }
            L = attr.find("add_fraction");
            if (L != attr.end()) {
                classAttribute.add_fraction = L->second;
            }

            // and merge it with the defaults inherited from the parent
            auto existingI = factory->m_attributes.find(entry.first);
            if (existingI != factory->m_attributes.end()) {
                classAttribute.combine(existingI->second);
            } else {
                Atlas::Message::Element value;
                classAttribute.combine(value);
                factory->m_attributes.emplace(entry.first, std::move(value));
            }
            // Store this value in the defaults for this class
            factory->m_classAttributes[entry.first] = std::move(classAttribute);
        }
    }

    return 0;
}

int EntityRuleHandler::check(const Atlas::Objects::Root& desc)
{
    assert(!desc->getParent().empty());
    if (desc->getObjtype() == "class") {
        return 0;
    }
    return -1;
}

int EntityRuleHandler::install(const std::string& name,
                               const std::string& parent,
                               const Atlas::Objects::Root& description,
                               std::string& dependent,
                               std::string& reason,
                               std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return installEntityClass(name, parent, description, dependent, reason, changes);
}

int EntityRuleHandler::update(const std::string& name,
                              const Atlas::Objects::Root& desc,
                              std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return modifyEntityClass(name, desc, changes);
}
