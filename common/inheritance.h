// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "log.h"
#include "operations.h"
#include "types.h"

#include <iostream>

void installStandardObjects();
void installCustomOperations();
void installCustomEntities();

class InheritanceException {
  private:
    const std::string parent;
  public:
    InheritanceException(const std::string & p) : parent(p) { }
    const std::string & getParent() { return parent; }
};

class Inheritance {
  protected:
    std::map<std::string, Atlas::Objects::Root *> atlasObjects;
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
    OpNo opEnumerate(const RootOperation & op) const;
    Atlas::Objects::Root * get(const std::string & parent);
    bool addChild(Atlas::Objects::Root * obj);
};

#endif // COMMON_INHERITANCE_H
