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

#include <boost/algorithm/string.hpp>

#include <set>

static const bool debug_flag = false;

using Atlas::Message::MapType;

TypeNode::TypeNode(const std::string& name)
    : m_name(name),
      m_parent(nullptr)
{
}

TypeNode::TypeNode(const std::string& name,
                   const Atlas::Objects::Root& d)
    : m_name(name),
      m_parent(nullptr)
{
    setDescription(d);
}

TypeNode::~TypeNode()
{
    for (auto entry: m_defaults) {
        delete entry.second;
    }
}


void TypeNode::setDescription(const Atlas::Objects::Root& description)
{
    //We need to split out any private and protected attributes
    m_privateDescription = description;
    m_protectedDescription = description.copy();
    m_publicDescription = description.copy();

    Atlas::Message::Element attributesElement;
    if (description->copyAttr("attributes", attributesElement) == 0 && attributesElement.isMap()) {
        auto& attributes = attributesElement.Map();

        auto protected_attributes = attributes;
        auto public_attributes = attributes;
        bool had_private_attributes = false;
        bool had_protected_attributes = false;

        for (auto& entry : attributes) {
            if (boost::starts_with(entry.first, "__")) {
                protected_attributes.erase(entry.first);
                public_attributes.erase(entry.first);
                had_private_attributes = true;
            } else if (boost::starts_with(entry.first, "_")) {
                public_attributes.erase(entry.first);
                had_protected_attributes = true;
            }
        }

        if (had_private_attributes) {
            m_protectedDescription->setAttr("attributes", protected_attributes);
            m_publicDescription->setAttr("attributes", public_attributes);
        } else if (had_protected_attributes) {
            m_publicDescription->setAttr("attributes", public_attributes);
        }
    }
}

void TypeNode::injectProperty(const std::string& name,
                              PropertyBase* p)
{
    auto existingI = m_defaults.find(name);
    if (existingI != m_defaults.end() && existingI->second != p) {
        delete existingI->second;
        m_defaults.erase(existingI);
    } else {
        m_defaults.emplace(name, p);
    }

    auto add_attribute_fn = [&](Atlas::Objects::Root& description) {
        Atlas::Message::Element attributesElement = Atlas::Message::MapType();
        if (description->hasAttr("attributes")) {
            attributesElement = description->getAttr("attributes");
        }
        Atlas::Message::Element propertyElement;
        p->get(propertyElement);
        attributesElement.Map()[name] = Atlas::Message::MapType{
            {"default", propertyElement}
        };
        description->setAttr("attributes", attributesElement);
    };

    add_attribute_fn(m_privateDescription);
    if (!boost::starts_with(name, "__")) {
        add_attribute_fn(m_protectedDescription);
    }
    if (!boost::starts_with(name, "_")) {
        add_attribute_fn(m_publicDescription);
    }

}

void TypeNode::addProperties(const MapType& attributes)
{
    for (auto entry : attributes) {
        PropertyBase* p = PropertyManager::instance().addProperty(entry.first,
                                                                  entry.second.getType());
        assert(p != nullptr);
        p->set(entry.second);
        p->addFlags(flag_class);
        p->install(this, entry.first);
        m_defaults[entry.first] = p;
    }
}

TypeNode::PropertiesUpdate TypeNode::updateProperties(const MapType& attributes)
{

    PropertiesUpdate propertiesUpdate;

    auto extractAttributesFn = [&](const  Atlas::Objects::Root& description, Atlas::Message::MapType& attributesMap){
        if (description->hasAttr("attributes")) {
            auto elem = description->getAttr("attributes");
            if (elem.isMap()) {
                attributesMap = std::move(elem.Map());
            } else {
                log(WARNING, String::compose("TypeNode '%1' had an 'attribute' element which wasn't a map.", m_name));
            }
        }
    };

    //Update the description
    Atlas::Message::MapType attributesMapPrivate;
    extractAttributesFn(m_privateDescription, attributesMapPrivate);

    Atlas::Message::MapType attributesMapProtected;
    extractAttributesFn(m_protectedDescription, attributesMapProtected);

    Atlas::Message::MapType attributesMapPublic;
    extractAttributesFn(m_publicDescription, attributesMapPublic);

    // Discover the default attributes which are no longer
    // present after the update.
    for (auto& entry : m_defaults) {
        //Don't remove ephemeral attributes.
        if (attributes.find(entry.first) == attributes.end() && !entry.second->hasFlags(per_ephem)) {
            debug(std::cout << entry.first << " removed" << std::endl;);
            propertiesUpdate.removedProps.insert(entry.first);
        }
    }

    // Remove the class properties for the default attributes that
    // no longer exist
    for (auto& entry : propertiesUpdate.removedProps) {
        auto M = m_defaults.find(entry);
        delete M->second;
        m_defaults.erase(M);
    }



    // Update the values of existing class properties, and add new class
    // properties for added default attributes.
    for (auto& entry : attributes) {
        PropertyBase* p;
        auto I = m_defaults.find(entry.first);
        if (I == m_defaults.end()) {
            p = PropertyManager::instance().addProperty(entry.first, entry.second.getType());
            assert(p != nullptr);
            p->addFlags(flag_class);
            p->install(this, entry.first);
            m_defaults[entry.first] = p;
            propertiesUpdate.newProps.emplace(entry.first);
            p->set(entry.second);

            Atlas::Message::MapType map_entry{
                {"default", entry.second}
            };

            attributesMapPrivate[entry.first] = map_entry;
            if (!boost::starts_with(entry.first, "__")) {
                attributesMapProtected[entry.first] = map_entry;
            }
            if (!boost::starts_with(entry.first, "_")) {
                attributesMapPublic[entry.first] = map_entry;
            }
        } else {
            Atlas::Message::Element oldVal;
            p = I->second;
            p->get(oldVal);
            if (oldVal != entry.second) {
                p->set(entry.second);
                propertiesUpdate.changedProps.emplace(entry.first);
                Atlas::Message::MapType map_entry{
                    {"default", entry.second}
                };

                attributesMapPrivate[entry.first] = map_entry;
                if (!boost::starts_with(entry.first, "__")) {
                    attributesMapProtected[entry.first] = map_entry;
                }
                if (!boost::starts_with(entry.first, "_")) {
                    attributesMapPublic[entry.first] = map_entry;
                }
            }
        }
    }

    m_privateDescription->setAttr("attributes", attributesMapPrivate);
    m_protectedDescription->setAttr("attributes", attributesMapProtected);
    m_publicDescription->setAttr("attributes", attributesMapPublic);

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

