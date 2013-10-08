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

#include "rulesets/PythonScriptFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"
#include "common/EntityKit.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/objectFactory.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Factories;

using String::compose;

static const bool debug_flag = false;

int ArchetypeRuleHandler::installArchetypeClass(const std::string & class_name,
        const std::string & parent, const Root & class_desc,
        std::string & dependent, std::string & reason)
{
    assert(class_name == class_desc->getId());

    // Get the new factory for this rule
    ArchetypeFactory * parent_factory =
            dynamic_cast<ArchetypeFactory*>(m_builder->getClassFactory(parent));
    if (parent_factory == 0) {
        debug(
                std::cout << "class \"" << class_name
                        << "\" has non existant parent \"" << parent
                        << "\". Waiting." << std::endl << std::flush
                ;);
        dependent = parent;
        reason = compose("Entity rule \"%1\" has parent \"%2\" which does "
                "not exist.", class_name, parent);
        return 1;
    }
    ArchetypeFactory * factory = parent_factory->duplicateFactory();
    if (factory == 0) {
        log(ERROR,
                compose(
                        "Attempt to install rule \"%1\" which has parent \"%2\" "
                                "which cannot be instantiated", class_name,
                        parent));
        return -1;
    }

    assert(factory->m_parent == parent_factory);

    if (populateArchetypeFactory(class_name, factory, class_desc->asMessage(),
            dependent, reason) != 0) {
        delete factory;
        return -1;
    }

    debug(
            std::cout << "INSTALLING " << class_name << ":" << parent
                    << std::endl << std::flush
            ;);

    // Install the factory in place.
    if (m_builder->installFactory(class_name, class_desc, factory) != 0) {
        delete factory;
        return -1;
    }

    factory->addProperties();

    // Add it as a child to its parent.
    parent_factory->m_children.insert(factory);

    return 0;
}

int ArchetypeRuleHandler::modifyArchetypeClass(const std::string & class_name,
        const Root & class_desc)
{
    assert(class_name == class_desc->getId());

    ArchetypeFactory * factory =
            dynamic_cast<ArchetypeFactory*>(m_builder->getClassFactory(
                    class_name));
    if (factory == 0) {
        log(ERROR, compose("Could not find factory for existing entity class "
                "\"%1\".", class_name));
        return -1;
    }
    assert(factory != 0);

    auto backup_entities = factory->m_entities;
    auto backup_thoughts = factory->m_thoughts;

    // Copy the defaults from the parent. In populateArchetypeFactory this may be
    // overriden with the defaults for this class.
    if (factory->m_parent != 0) {
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

    std::string dependent, reason;
    if (populateArchetypeFactory(class_name, factory, class_desc->asMessage(),
            dependent, reason) != 0) {
        factory->m_entities = backup_entities;
        factory->m_thoughts = backup_thoughts;
        return -1;
    }

    factory->updateProperties();

    return 0;
}

int ArchetypeRuleHandler::populateArchetypeFactory(
        const std::string & class_name, ArchetypeFactory * factory,
        const MapType & class_desc, std::string & dependent,
        std::string & reason)
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
//    // Store the default attribute for entities create by this rule.
//    J = class_desc.find("attributes");
//    if (J != Jend && J->second.isMap()) {
//        const MapType & attrs = J->second.asMap();
//        MapType::const_iterator Kend = attrs.end();
//        for (MapType::const_iterator K = attrs.begin(); K != Kend; ++K) {
//            if (!K->second.isMap()) {
//                log(ERROR, compose("Attribute description in rule %1 is not a "
//                                   "map.", class_name));
//                continue;
//            }
//            const MapType & attr = K->second.asMap();
//            MapType::const_iterator L = attr.find("default");
//            if (L != attr.end()) {
//                // Store this value in the defaults for this class
//                factory->m_classAttributes[K->first] = L->second;
//                // and merge it with the defaults inherited from the parent
//                factory->m_attributes[K->first] = L->second;
//            }
//        }
//    }
//
    auto entitiesI = class_desc.find("entities");
    if (entitiesI != class_desc.end() && entitiesI->second.isList()) {
        for (auto& I : entitiesI->second.asList()) {
            if (I.isMap()) {
                factory->m_entities.push_back(I.asMap());
            }
        }
    }

    auto thoughtsI = class_desc.find("thoughts");
    if (thoughtsI != class_desc.end() && thoughtsI->second.isList()) {
        factory->m_thoughts = thoughtsI->second.asList();
//        for (auto& I : thoughtsI->second.asList()) {
//            if (I.isMap()) {
//                RootEntity thought = smart_dynamic_cast<RootEntity>(
//                        Factories::instance()->createObject(I.asMap()));
//                if (!thought.isValid()) {
//                    log(ERROR, "Thought definition is not in Entity format.");
//                } else {
//                    factory->m_thoughts.push_back(I);
//                }
//            }
//        }
    }
    // Check whether it should be available to players as a playable character.
    auto playableI = class_desc.find("playable");
    if (playableI != class_desc.end() && playableI->second.isInt()) {
        Player::playableTypes.insert(class_name);
    }

    return 0;
}

int ArchetypeRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(!desc->getParents().empty());
    if (desc->getObjtype() != "archetype") {
        return -1;
    }
    return 0;
}

int ArchetypeRuleHandler::install(const std::string & name,
        const std::string & parent, const Atlas::Objects::Root & description,
        std::string & dependent, std::string & reason)
{
    return installArchetypeClass(name, parent, description, dependent, reason);
}

int ArchetypeRuleHandler::update(const std::string & name,
        const Atlas::Objects::Root & desc)
{
    return modifyArchetypeClass(name, desc);
}
