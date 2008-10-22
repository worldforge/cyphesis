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

// $Id$

#ifndef COMMON_TYPE_NODE_H
#define COMMON_TYPE_NODE_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

class PropertyBase;

typedef std::map<std::string, PropertyBase *> PropertyDict;

class TypeNode {
  protected:
    std::string m_name;

    PropertyDict m_defaults;

    Atlas::Objects::Root m_description;

    const TypeNode * m_parent;
  public:
    TypeNode();
    ~TypeNode();

    const std::string & name() const {
        return m_name;
    }

    std::string & name() {
        return m_name;
    }

    const PropertyDict & defaults() const {
        return m_defaults;
    }

    PropertyDict & defaults() {
        return m_defaults;
    }

    Atlas::Objects::Root & description() {
        return m_description;
    }

    const Atlas::Objects::Root & description() const {
        return m_description;
    }

    const TypeNode * parent() const {
        return m_parent;
    }

    void setParent(const TypeNode * parent) {
        m_parent = parent;
    }
};

#endif // COMMON_TYPE_NODE_H
