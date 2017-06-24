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

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(0)
{
}

TypeNode::~TypeNode()
{
    PropertyDict::const_iterator I = m_defaults.begin();
    PropertyDict::const_iterator Iend = m_defaults.end();
    for (; I != Iend; ++I) {
        delete I->second;
    }
}

void TypeNode::addProperty(const std::string & name,
                           PropertyBase * p)
{
    m_defaults[name] = p;
}

void TypeNode::addProperties(const MapType & attributes)
{
    MapType::const_iterator J = attributes.begin();
    MapType::const_iterator Jend = attributes.end();
    PropertyBase * p;
    for (; J != Jend; ++J) {
        p = PropertyManager::instance()->addProperty(J->first,
                                                     J->second.getType());
        assert(p != 0);
        p->set(J->second);
        p->setFlags(flag_class);
        m_defaults[J->first] = p;
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
        PropertyDict::iterator M = m_defaults.find(entry);
        delete M->second;
        m_defaults.erase(M);
    }

    // Update the values of existing class properties, and add new class
    // properties for added default attributes.
    PropertyBase * p;
    for (auto& entry : attributes) {
        auto I = m_defaults.find(entry.first);
        if (I == m_defaults.end()) {
            p = PropertyManager::instance()->addProperty(entry.first, entry.second.getType());
            assert(p != nullptr);
            p->setFlags(flag_class);
            m_defaults[entry.first] = p;
        } else {
            p = I->second;
        }
        p->set(entry.second);
    }
}

bool TypeNode::isTypeOf(const std::string & base_type) const
{
    const TypeNode * node = this;
    do {
        if (node->name() == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != 0);
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
    } while (node != 0);
    return false;
}
