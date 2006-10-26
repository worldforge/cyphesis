// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

// $Id: inheritance.h,v 1.31 2006-10-26 00:48:05 alriddoch Exp $

#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "types.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

void installStandardObjects();
void installCustomOperations();
void installCustomEntities();

typedef std::map<std::string, Atlas::Objects::Root> RootDict;

/// \brief Class to manage the inheritance tree for in-game entity types
class Inheritance {
  protected:
    const Atlas::Objects::Root noClass;
    RootDict atlasObjects;
    OpNoDict opLookup;

    static Inheritance * m_instance;

    Inheritance();

    void flush();
  public:
    static Inheritance & instance();
    static void clear();

    void opInstall(const std::string & op, OpNo no) {
        opLookup[op] = no;
    }

    OpNo opEnumerate(const std::string & parent) const;
    OpNo opEnumerate(const Operation &) const;
    const Atlas::Objects::Root & getClass(const std::string & parent);
    bool hasClass(const std::string & parent);
    int addChild(const Atlas::Objects::Root & obj);
    bool isTypeOf(const std::string &, const std::string &) const;
};

Atlas::Objects::Root atlasOpDefinition(const std::string & name,
                                       const std::string & parent);
Atlas::Objects::Root atlasClass(const std::string & name,
                                const std::string & parent);

#endif // COMMON_INHERITANCE_H
