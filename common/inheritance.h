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

    void flush() {
       std::map<std::string, Atlas::Objects::Root *>::const_iterator I;
       for(I = atlasObjects.begin(); I != atlasObjects.end(); ++I) {
           delete I->second;
       }
       atlasObjects.clear();
    }
  public:
    static Inheritance & instance() {
        if (m_instance == NULL) {
            m_instance = new Inheritance();
            installStandardObjects();
            installCustomOperations();
            installCustomEntities();
        }
        return *m_instance;
    }

    static void clear() {
        if (m_instance != NULL) {
            m_instance->flush();
            delete m_instance;
        }
    }

    void opInstall(const std::string & op, OpNo no) {
        opLookup[op] = no;
    }

    OpNo opEnumerate(const std::string & parent) const {
        OpNoDict::const_iterator I = opLookup.find(parent);
        if (I != opLookup.end()) {
            return I->second;
        } else {
            return OP_INVALID;
        }
    }

    OpNo opEnumerate(const RootOperation & op) const {
        const Atlas::Message::Object::ListType & parents = op.GetParents();
        if (parents.size() != 1) {
            log(ERROR, "op with no parents");
        }
        if (!parents.begin()->IsString()) {
            log(ERROR, "op with non-string parent");
        }
        const std::string & parent = parents.begin()->AsString();
        return opEnumerate(parent);
    }

    Atlas::Objects::Root * get(const std::string & parent) {
        std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
        if (I == atlasObjects.end()) {
            return NULL;
        }
        return I->second;
    }

    bool addChild(Atlas::Objects::Root * obj) {
        const std::string & child = obj->GetId();
        const std::string & parent = obj->GetParents().front().AsString();
        if (atlasObjects.find(child) != atlasObjects.end()) {
            std::string msg = std::string("Installing type ") + child 
                            + "(" + parent + ") which was already installed";
            log(WARNING, msg.c_str());
            delete obj;
            return true;
        }
        std::map<std::string, Atlas::Objects::Root *>::const_iterator I = atlasObjects.find(parent);
        if (I == atlasObjects.end()) {
            throw InheritanceException(parent);
        }
        Atlas::Message::Object::ListType children(1, child);
        if (I->second->HasAttr("children")) {
            children = I->second->GetAttr("children").AsList();
            children.push_back(child);
        }
        I->second->SetAttr("children", Atlas::Message::Object(children));
        atlasObjects[child] = obj;
        return false;
    }

};

#endif // COMMON_INHERITANCE_H
