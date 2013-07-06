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


#ifndef COMMON_TYPE_NODE_H
#define COMMON_TYPE_NODE_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

class PropertyBase;

typedef std::map<std::string, PropertyBase *> PropertyDict;

/// \brief Entry in the type hierarchy for in-game entity classes.
class TypeNode {
  protected:
    /// \brief name
    const std::string m_name;

    /// \brief property defaults
    PropertyDict m_defaults;

    /// \brief type description
    Atlas::Objects::Root m_description;

    /// \brief parent node
    const TypeNode * m_parent;
  public:
    TypeNode(const std::string &);
    TypeNode(const std::string &, const Atlas::Objects::Root &);
    ~TypeNode();

    void addProperty(const std::string &, PropertyBase *);

    /// \brief add the class properties for this type from Atlas attributes
    void addProperties(const Atlas::Message::MapType & attributes);

    /// \brief update the class properties for this type from Atlas attributes
    void updateProperties(const Atlas::Message::MapType & attributes);

    /// \brief check if this type inherits from another
    bool isTypeOf(const std::string & base_type) const;

    /// \brief check if this type inherits from another
    bool isTypeOf(const TypeNode * base_type) const;

    /// \brief const accessor for name
    const std::string & name() const {
        return m_name;
    }

    /// \brief const accessor for property defaults
    const PropertyDict & defaults() const {
        return m_defaults;
    }

    /// \brief accessor for type description
    Atlas::Objects::Root & description() {
        return m_description;
    }

    /// \brief const accessor for type description
    const Atlas::Objects::Root & description() const {
        return m_description;
    }

    /// \brief const accessor for parent node
    const TypeNode * parent() const {
        return m_parent;
    }

    /// \brief set the parent node
    void setParent(const TypeNode * parent) {
        m_parent = parent;
    }
};

#endif // COMMON_TYPE_NODE_H
