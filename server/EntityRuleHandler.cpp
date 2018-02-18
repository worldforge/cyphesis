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

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;


class Character;

class Creator;

class Entity;

class Plant;

class Stackable;

class Thing;

class World;

extern template
class EntityFactory<Character>;

extern template
class EntityFactory<Creator>;

extern template
class EntityFactory<Thing>;

extern template
class EntityFactory<Plant>;

extern template
class EntityFactory<Stackable>;

extern template
class EntityFactory<World>;

static const bool debug_flag = false;


EntityRuleHandler::EntityRuleHandler(EntityBuilder * eb)
    : m_builder(eb)
{
    installStandardRules();
}

void EntityRuleHandler::installStandardRules()
{
    auto composeDeclaration = [](std::string class_name, std::string parent, Atlas::Message::MapType rawAttributes) {

        Atlas::Objects::Root decl;
        decl->setObjtype("class");
        decl->setId(class_name);
        decl->setParent(parent);

        Atlas::Message::MapType composed;
        for (const auto& entry : rawAttributes) {
            composed[entry.first] = Atlas::Message::MapType{
                {"default", entry.second},
                {"visibility", "public"}
            };
        }

        decl->setAttr("attributes", composed);
        return decl;
    };

    std::string dependent, reason;

    {
        Atlas::Objects::Root decl = composeDeclaration("world", "game_entity", {});
        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<World>());
    }

    {
        Atlas::Objects::Root decl = composeDeclaration("thing", "game_entity", {
            {"mode", "planted"}
        });
        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Thing>());
    }

    {
        Atlas::Objects::Root decl = composeDeclaration("character", "thing", {
            {"density", 125}
        });
        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Character>());
    }

    {
        Atlas::Objects::Root decl = composeDeclaration("creator", "character", {
            {"transient", -1},
            {"solid", 0},
            //Creator agents should have a bbox, so that they easily can be made solid for testing purposes.
            {"bbox", ListType {-.5, 0, -0.5, .5, 1, .5}},
            {"geometry", MapType {{"type", "sphere"}}},
            {"friction", 10.0},
            {"angularfactor", ListType {0, 0, 0}},
            {"mass", 1.0},
            //Creator agents should by default move quickly.
            {"speed-ground", 15},
            {"speed-water", 15},
            {"speed-flight", 15},
            {"present", "dural/items/misc/flame/creator.modeldef"},
        });

        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Creator>());
    }

    {
        Atlas::Objects::Root decl = composeDeclaration("plant", "thing", {
            {"friction", 1.0},
            {"mode", "planted"},
            {"status", 1.0f},
            //Plants should by default be represented by an upright cylinder.
            {"geometry", MapType {{"type", "cylinder-y"}}},
        });
        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Plant>());
    }

    {
        Atlas::Objects::Root decl = composeDeclaration("stackable", "thing", {});
        installEntityClass(decl->getId(), decl->getParent(), decl, dependent, reason, new EntityFactory<Stackable>());
    }
}

int EntityRuleHandler::installEntityClass(const std::string & class_name,
                                          const std::string & parent,
                                          const Root & class_desc,
                                          std::string & dependent,
                                          std::string & reason)
{
    assert(class_name == class_desc->getId());

    // Get the new factory for this rule
    EntityFactoryBase * parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(parent));
    if (parent_factory == nullptr) {
        debug(std::cout << "class \"" << class_name
                        << "\" has non existant parent \"" << parent
                        << "\". Waiting." << std::endl << std::flush;);
        dependent = parent;
        reason = compose("Entity rule \"%1\" has parent \"%2\" which does "
                         "not exist.", class_name, parent);
        return 1;
    }
    EntityFactoryBase * factory = parent_factory->duplicateFactory();
    assert(factory->m_parent == parent_factory);

    return installEntityClass(class_name, parent, class_desc, dependent, reason, factory);
}

int EntityRuleHandler::installEntityClass(const std::string & class_name,
                                          const std::string & parent,
                                          const Root & class_desc,
                                          std::string & dependent,
                                          std::string & reason,
                                          EntityFactoryBase* factory)
{
    // Get the new factory for this rule
    EntityFactoryBase * parent_factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(parent));

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

    if (parent_factory) {
        // Add it as a child to its parent.
        parent_factory->m_children.insert(factory);
    }

    return 0;
}
int EntityRuleHandler::modifyEntityClass(const std::string & class_name,
                                         const Root & class_desc)
{
    assert(class_name == class_desc->getId());

    EntityFactoryBase* factory = dynamic_cast<EntityFactoryBase*>(m_builder->getClassFactory(class_name));
    if (factory == 0) {
        log(ERROR, compose("Could not find factory for existing entity class "
                           "\"%1\".", class_name));
        return -1;
    }
    assert(factory != 0);
    
    MapType backup_attributes = factory->m_attributes,
            backup_class_attributes = factory->m_classAttributes;

    // Copy the defaults from the parent. In populateEntityFactory this may be
    // overridden with the defaults for this class.
    if (factory->m_parent != 0) {
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

    factory->updateProperties();

    return 0;
}


int EntityRuleHandler::populateEntityFactory(const std::string & class_name,
                                             EntityFactoryBase * factory,
                                             const MapType & class_desc,
                                             std::string & dependent,
                                             std::string & reason)
{
    // assert(class_name == class_desc->getId());
    // Establish whether this rule has an associated script, and
    // if so, use it.
    MapType::const_iterator J = class_desc.find("script");
    MapType::const_iterator Jend = class_desc.end();
    if (J != Jend && J->second.isMap()) {
        const MapType & script = J->second.asMap();
        std::string script_package;
        std::string script_class;
        if (getScriptDetails(script, class_name, "Entity",
                             script_package, script_class) != 0) {
            return -1;
        }
        if (factory->m_scriptFactory == 0 ||
            factory->m_scriptFactory->package() != script_package) {
            PythonScriptFactory<LocatedEntity> * psf =
                  new PythonScriptFactory<LocatedEntity>(script_package,
                                                         script_class);
            if (psf->setup() == 0) {
                delete factory->m_scriptFactory;
                factory->m_scriptFactory = psf;
            } else {
                log(ERROR, compose("Python class \"%1.%2\" failed to load",
                                   script_package, script_class));
                delete psf;
                return -1;
            }
        } else {
            // FIXME If this fails, that's bad.
            factory->m_scriptFactory->refreshClass();
        }
    }

    // Store the default attribute for entities create by this rule.
    J = class_desc.find("attributes");
    if (J != Jend && J->second.isMap()) {
        const MapType & attrs = J->second.asMap();
        MapType::const_iterator Kend = attrs.end();
        for (MapType::const_iterator K = attrs.begin(); K != Kend; ++K) {
            if (!K->second.isMap()) {
                log(ERROR, compose("Attribute description in rule %1 is not a "
                                   "map.", class_name));
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

int EntityRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(desc->getParent() != "");
    if (desc->getObjtype() != "class") {
        return -1;
    }
    return m_builder->isTask(desc->getParent()) ? -1 : 0;
}

int EntityRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason)
{
    return installEntityClass(name, parent, description, dependent, reason);
}

int EntityRuleHandler::update(const std::string & name,
                            const Atlas::Objects::Root & desc)
{
    return modifyEntityClass(name, desc);
}
