// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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


#include "TypeNode.h"

#include "debug.h"
#include "Property.h"
#include "PropertyManager.h"
#include "compose.hpp"
#include "log.h"

#include <set>
#include <utility>

static const bool debug_flag = false;

using Atlas::Message::MapType;

TypeNode::TypeNode(std::string name)
    : m_name(std::move(name)),
      m_parent(nullptr)
{
}

TypeNode::TypeNode(std::string name,
                   const Atlas::Objects::Root& d)
    : m_name(std::move(name)),
      m_parent(nullptr)
{
    setDescription(d);
}

TypeNode::~TypeNode()
{
    m_defaults.clear();
}


void TypeNode::setDescription(const Atlas::Objects::Root& description)
{
    //We need to split out any private and protected attributes
    m_privateDescription = description;
    m_protectedDescription = description.copy();
    // Regular clients (protected and public) don't need to see children.
    // We want to allow for some mystery about the different kinds of entities in the world.
    m_protectedDescription->removeAttr("children");
    m_protectedDescription->removeAttr("attributes");
    m_publicDescription = description.copy();
    m_publicDescription->removeAttr("children");
    m_publicDescription->removeAttr("attributes");
    m_privateDescription->setAttr("access", "private");
    m_protectedDescription->setAttr("access", "protected");
    m_publicDescription->setAttr("access", "public");

}

TypeNode::PropertiesUpdate TypeNode::injectProperty(const std::string& name,
                              std::unique_ptr<PropertyBase> prop)
{
    auto p = prop.get();
    TypeNode::PropertiesUpdate update;
    auto existingI = m_defaults.find(name);
    if (existingI != m_defaults.end() && existingI->second.get() != prop.get()) {
        existingI->second = std::move(prop);
        update.changedProps.insert(name);
    } else {
        m_defaults.emplace(name, std::move(prop));
        update.newProps.insert(name);
    }

    auto add_attribute_fn = [&](Atlas::Objects::Root& description) {
        Atlas::Message::Element propertiesElement = Atlas::Message::MapType();
        if (description->hasAttr("properties")) {
            propertiesElement = description->getAttr("properties");
        }
        Atlas::Message::Element propertyElement;
        p->get(propertyElement);
        propertiesElement.Map()[name] =  propertyElement;
        description->setAttr("properties", std::move(propertiesElement));
    };

    add_attribute_fn(m_privateDescription);
    if (!p->hasFlags(visibility_private)) {
        add_attribute_fn(m_protectedDescription);
    }
    if (!p->hasFlags(visibility_non_public)) {
        add_attribute_fn(m_publicDescription);
    }

    return update;

}

void TypeNode::addProperties(const MapType& attributes)
{
    for (const auto& entry : attributes) {
        auto p = PropertyManager::instance().addProperty(entry.first,
                                                                  entry.second.getType());
        assert(p != nullptr);
        p->set(entry.second);
        p->addFlags(flag_class);
        p->install(this, entry.first);
        m_defaults[entry.first] = std::move(p);
    }
}

TypeNode::PropertiesUpdate TypeNode::updateProperties(const MapType& attributes)
{

    PropertiesUpdate propertiesUpdate;

    //Update the description
    Atlas::Message::MapType attributesMapPrivate;
    Atlas::Message::MapType attributesMapProtected;
    Atlas::Message::MapType attributesMapPublic;

    // Discover the default attributes which are no longer
    // present after the update.
    for (auto& entry : m_defaults) {
        //Don't remove ephemeral attributes.
        if (attributes.find(entry.first) == attributes.end() && !entry.second->hasFlags(persistence_ephem)) {
            debug(std::cout << entry.first << " removed" << std::endl;)
            propertiesUpdate.removedProps.insert(entry.first);
        }
    }

    // Remove the class properties for the default attributes that
    // no longer exist
    for (auto& entry : propertiesUpdate.removedProps) {
        auto M = m_defaults.find(entry);
        m_defaults.erase(M);
    }



    // Update the values of existing class properties, and add new class
    // properties for added default attributes.
    for (auto& entry : attributes) {
        auto I = m_defaults.find(entry.first);
        if (I == m_defaults.end()) {
            auto p = PropertyManager::instance().addProperty(entry.first, entry.second.getType());
            assert(p != nullptr);
            p->addFlags(flag_class);
            p->install(this, entry.first);
            propertiesUpdate.newProps.emplace(entry.first);
            p->set(entry.second);

            attributesMapPrivate[entry.first] = entry.second;
            if (!p->hasFlags(visibility_private)) {
                attributesMapProtected[entry.first] = entry.second;
            }
            if (!p->hasFlags(visibility_non_public)) {
                attributesMapPublic[entry.first] = entry.second;
            }
            m_defaults[entry.first] = std::move(p);
        } else {
            Atlas::Message::Element oldVal;
            auto& p = I->second;
            p->get(oldVal);
            if (oldVal != entry.second) {
                p->set(entry.second);
                propertiesUpdate.changedProps.emplace(entry.first);
            }
            attributesMapPrivate[entry.first] = entry.second;
            if (!p->hasFlags(visibility_private)) {
                attributesMapProtected[entry.first] = entry.second;
            }
            if (!p->hasFlags(visibility_non_public)) {
                attributesMapPublic[entry.first] = entry.second;
            }
        }
    }

    m_privateDescription->setAttr("properties", std::move(attributesMapPrivate));
    m_protectedDescription->setAttr("properties", std::move(attributesMapProtected));
    m_publicDescription->setAttr("properties", std::move(attributesMapPublic));

    return propertiesUpdate;
}

bool TypeNode::isTypeOf(const std::string& base_type) const
{
    const TypeNode* node = this;
    do {
        if (node->name() == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != nullptr);
    return false;
}

bool TypeNode::isTypeOf(const TypeNode* base_type) const
{
    const TypeNode* node = this;
    do {
        if (node == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != nullptr);
    return false;
}

Atlas::Objects::Root& TypeNode::description(Visibility visibility)
{
    switch (visibility) {
        case Visibility::PROTECTED:
            return m_protectedDescription;
        case Visibility::PRIVATE:
            return m_privateDescription;
        case Visibility::PUBLIC:
        default:
            return m_publicDescription;
    }
}

const Atlas::Objects::Root& TypeNode::description(Visibility visibility) const
{
    switch (visibility) {
        case Visibility::PROTECTED:
            return m_protectedDescription;
        case Visibility::PRIVATE:
            return m_privateDescription;
        case Visibility::PUBLIC:
        default:
            return m_publicDescription;
    }
}


