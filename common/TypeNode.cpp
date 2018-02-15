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

#include <set>

static const bool debug_flag = false;

using Atlas::Message::MapType;

TypeNode::TypeNode(const std::string & name) : m_name(name),
                                               m_parent(nullptr)
{
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(nullptr)
{
}

TypeNode::~TypeNode()
{
    for (auto entry: m_defaults) {
        delete entry.second;
    }
}

void TypeNode::injectProperty(const std::string& name,
                              PropertyBase* p)
{
    auto existingI = m_defaults.find(name);
    if (existingI != m_defaults.end()) {
        delete existingI->second;
        m_defaults.erase(existingI);
    }
    m_defaults.emplace(name, p);
    Atlas::Message::Element attributesElement = Atlas::Message::MapType();
    if (m_description->hasAttr("attributes")) {
        attributesElement = m_description->getAttr("attributes");
    }
    Atlas::Message::Element propertyElement;
    p->get(propertyElement);
    attributesElement.Map()[name] = Atlas::Message::MapType{
        {"visibility", "public"},
        {"default", propertyElement}
    };
    m_description->setAttr("attributes", attributesElement);
}

void TypeNode::addProperties(const MapType & attributes)
{
    for (auto entry : attributes) {
        PropertyBase * p = PropertyManager::instance()->addProperty(entry.first,
                                                                    entry.second.getType());
        assert(p != nullptr);
        p->set(entry.second);
        p->setFlags(flag_class);
        p->install(this, entry.first);
        m_defaults[entry.first] = p;
    }
}

void TypeNode::updateProperties(const MapType & attributes)
{
    // Discover the default attributes which are no longer
    // present after the update.
    std::set<std::string> removed_properties;
    for (auto& entry : m_defaults) {
        if (attributes.find(entry.first) == attributes.end()) {
            debug( std::cout << entry.first << " removed" << std::endl; );
            removed_properties.insert(entry.first);
        }
    }

    // Remove the class properties for the default attributes that
    // no longer exist
    for (auto& entry : removed_properties) {
        auto M = m_defaults.find(entry);
        delete M->second;
        m_defaults.erase(M);
    }

    // Update the values of existing class properties, and add new class
    // properties for added default attributes.
    std::map<std::string, PropertyBase*> newProps;
    for (auto& entry : attributes) {
        PropertyBase * p;
        auto I = m_defaults.find(entry.first);
        if (I == m_defaults.end()) {
            p = PropertyManager::instance()->addProperty(entry.first, entry.second.getType());
            assert(p != nullptr);
            p->setFlags(flag_class);
            p->install(this, entry.first);
            m_defaults[entry.first] = p;
            newProps.emplace(entry.first, p);
        } else {
            p = I->second;
        }
        p->set(entry.second);
    }

    //Update the description
    Atlas::Message::Element attributesElement = Atlas::Message::MapType();
    if (m_description->hasAttr("attributes")) {
        attributesElement = m_description->getAttr("attributes");
    }
    Atlas::Message::Element propertyElement;
    for (auto entry : newProps) {
        entry.second->get(propertyElement);
        attributesElement.Map()[entry.first] = Atlas::Message::MapType{
            {"visibility", "public"},
            {"default", propertyElement}
        };
    }
    m_description->setAttr("attributes", attributesElement);

}

bool TypeNode::isTypeOf(const std::string & base_type) const
{
    const TypeNode * node = this;
    do {
        if (node->name() == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != nullptr);
    return false;
}

bool TypeNode::isTypeOf(const TypeNode * base_type) const
{
    const TypeNode * node = this;
    do {
        if (node == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != nullptr);
    return false;
}
