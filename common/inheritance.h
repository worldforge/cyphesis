// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "log.h"
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

class OpFactoryBase {
  public:
    virtual ~OpFactoryBase();

    virtual RootOperation * newOperation() = 0;
};

template <class OpClass>
class OpFactory : public OpFactoryBase {
  public:
    virtual RootOperation * newOperation();
};

class GenericOpFactory : public OpFactoryBase {
  private:
    std::string m_opType;
  public:
    explicit GenericOpFactory(const std::string & opType);

    virtual RootOperation * newOperation();
};

class Inheritance {
  protected:
    std::map<std::string, Atlas::Objects::Root *> atlasObjects;
    OpNoDict opLookup;
    std::map<std::string, OpFactoryBase *> opFactories;

    static Inheritance * m_instance;

    Inheritance();

    void flush();
  public:
    static Inheritance & instance();
    static void clear();

    void opInstall(const std::string & op, OpNo no, OpFactoryBase * f) {
        opLookup[op] = no;
        opFactories[op] = f;
    }

    OpNo opEnumerate(const std::string & parent) const;
    OpNo opEnumerate(const RootOperation & op) const;
    Atlas::Objects::Root * get(const std::string & parent);
    int addChild(Atlas::Objects::Root * obj);
    RootOperation * newOperation(const std::string & op_type);
};

#endif // COMMON_INHERITANCE_H
