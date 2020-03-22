// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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

#include "ArchetypeRuleHandler.h"

#include "EntityBuilder.h"
#include "Player.h"
#include "ArchetypeFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Factories.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

using String::compose;

static const bool debug_flag = false;

int ArchetypeRuleHandler::installArchetypeClass(const std::string& class_name,
                                                const std::string& parent, const Root& class_desc,
                                                std::string& dependent, std::string& reason)
{
    assert(class_name == class_desc->getId());

    // Get the new factory for this rule
    auto* parent_factory = dynamic_cast<ArchetypeFactory*>(m_builder.getClassFactory(parent));
    if (parent_factory == nullptr) {
        debug_print(
             "class \"" << class_name
                      << "\" has non existent parent \"" << parent
                      << "\". Waiting.")
        dependent = parent;
        reason = compose("Entity rule \"%1\" has parent \"%2\" which does "
                         "not exist.", class_name, parent);
        return 1;
    }
    auto factory = parent_factory->duplicateFactory();
    if (!factory) {
        log(ERROR,
            compose(
                "Attempt to install rule \"%1\" which has parent \"%2\" "
                "which cannot be instantiated", class_name,
                parent));
        return -1;
    }

    assert(factory->m_parent == parent_factory);

    if (populateArchetypeFactory(class_name, factory.get(), class_desc->asMessage(),
                                 dependent, reason) != 0) {
        return -1;
    }

    debug_print("INSTALLING " << class_name << ":" << parent)

    auto factoryPtr = factory.get();
    // Install the factory in place.
    if (m_builder.installFactory(class_name, class_desc, std::move(factory)) != 0) {
        return -1;
    }

    factoryPtr->addProperties(m_propertyManager);

    // Add it as a child to its parent.
    parent_factory->m_children.insert(factoryPtr);

    return 0;
}

int ArchetypeRuleHandler::modifyArchetypeClass(const std::string& class_name,
                                               const Root& class_desc, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    auto factory =
        dynamic_cast<ArchetypeFactory*>(m_builder.getClassFactory(class_name));
    if (factory == nullptr) {
        log(ERROR, compose("Could not find factory for existing entity class "
                           "\"%1\".", class_name));
        return -1;
    }
    assert(factory != nullptr);

    auto backup_entities = factory->m_entities;
    auto backup_thoughts = factory->m_thoughts;

    // Copy the defaults from the parent. In populateArchetypeFactory this may be
    // overridden with the defaults for this class.
    if (factory->m_parent != nullptr) {
        factory->m_entities = factory->m_parent->m_entities;
        factory->m_thoughts = factory->m_parent->m_thoughts;
    } else {
        // This is non fatal, but nice to know it has happened.
        // This should only happen if the client attempted to modify the
        // type data for a core hard coded type.
        log(ERROR,
            compose(
                "ArchetypeRuleHandler::modifyEntityClass: \"%1\" modified "
                "by client, but has no parent factory.",
                class_name));
        factory->m_entities.clear();
        factory->m_thoughts.clear();
    }

    factory->m_classEntities.clear();
    factory->m_classThoughts.clear();

    std::string dependent, reason;
    if (populateArchetypeFactory(class_name, factory, class_desc->asMessage(),
                                 dependent, reason) != 0) {
        factory->m_entities = backup_entities;
        factory->m_thoughts = backup_thoughts;
        return -1;
    }

    factory->updateProperties(changes, m_propertyManager);

    return 0;
}

int ArchetypeRuleHandler::populateArchetypeFactory(
    const std::string& class_name, ArchetypeFactory* factory,
    const MapType& class_desc, std::string& dependent,
    std::string& reason)
{
    // assert(class_name == class_desc->getId());
    // Establish whether this rule has an associated script, and
    // if so, use it.
//    MapType::const_iterator J = class_desc.find("script");
//    MapType::const_iterator Jend = class_desc.end();
//    if (J != Jend && J->second.isMap()) {
//        const MapType & script = J->second.asMap();
//        std::string script_package;
//        std::string script_class;
//        if (getScriptDetails(script, class_name, "Archetype",
//                             script_package, script_class) != 0) {
//            return -1;
//        }
//        if (factory->m_scriptFactory == 0 ||
//            factory->m_scriptFactory->package() != script_package) {
//            PythonScriptFactory<LocatedEntity> * psf =
//                  new PythonScriptFactory<LocatedEntity>(script_package,
//                                                         script_class);
//            if (psf->setup() == 0) {
//                delete factory->m_scriptFactory;
//                factory->m_scriptFactory = psf;
//            } else {
//                log(ERROR, compose("Python class \"%1.%2\" failed to load",
//                                   script_package, script_class));
//                delete psf;
//                return -1;
//            }
//        } else {
//            // FIXME If this fails, that's bad.
//            factory->m_scriptFactory->refreshClass();
//        }
//    }
//

    auto entitiesI = class_desc.find("entities");
    if (entitiesI != class_desc.end() && entitiesI->second.isList()) {
        for (auto& I : entitiesI->second.asList()) {
            if (I.isMap()) {
                const MapType& map = I.asMap();
                std::string id;
                auto idI = map.find("id");
                if (idI != map.end() && idI->second.isString()) {
                    id = idI->second.asString();
                }

                MapType& entity = factory->m_entities[id];
                MapType& classEntity = factory->m_classEntities[id];
                for (auto& entry : map) {
                    entity[entry.first] = entry.second;
                    classEntity[entry.first] = entry.second;
                }
            }
        }
    }

    auto thoughtsI = class_desc.find("thoughts");
    if (thoughtsI != class_desc.end() && thoughtsI->second.isList()) {
        const ListType& thoughts = thoughtsI->second.asList();
        factory->m_thoughts.insert(factory->m_thoughts.end(), thoughts.begin(),
                                   thoughts.end());
        factory->m_classThoughts.insert(factory->m_classThoughts.end(),
                                        thoughts.begin(), thoughts.end());
    }

    return 0;
}

int ArchetypeRuleHandler::check(const Atlas::Objects::Root& desc)
{
    assert(!desc->getParent().empty());
    if (desc->getObjtype() != "archetype") {
        return -1;
    }
    return 0;
}

int ArchetypeRuleHandler::install(const std::string& name,
                                  const std::string& parent,
                                  const Atlas::Objects::Root& description,
                                  std::string& dependent,
                                  std::string& reason,
                                  std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return installArchetypeClass(name, parent, description, dependent, reason);
}

int ArchetypeRuleHandler::update(const std::string& name,
                                 const Atlas::Objects::Root& desc,
                                 std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    return modifyArchetypeClass(name, desc, changes);
}
