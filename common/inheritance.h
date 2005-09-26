// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

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

    // OpNo opEnumerate(const std::string & parent) const;
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
