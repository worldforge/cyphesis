// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "operations.h"

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

    static Inheritance * m_instance;

    Inheritance();
  public:
    static Inheritance & instance() {
        if (m_instance == NULL) {
            m_instance = new Inheritance();
        }
        return *m_instance;
    }

    Atlas::Objects::Root * get(const std::string & parent) {
        std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
        if (I == atlasObjects.end()) {
            throw InheritanceException(parent);
        }
        return I->second;
    }

    void addChild(const std::string & parent, const std::string & child) {
        std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
        if (I == atlasObjects.end()) {
            throw InheritanceException(parent);
        }
        Atlas::Message::Object::ListType & children = I->second->GetAttr("children").AsList();
        children.push_back(child);
    }
};

extern Inheritance inheritance;

#endif // COMMON_INHERITANCE_H
