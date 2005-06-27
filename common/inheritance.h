// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_INHERITANCE_H
#define COMMON_INHERITANCE_H

#include "types.h"

#include <iostream>

namespace Atlas {
  namespace Objects {
    class Root;
  }
}

void installStandardObjects();
void installCustomOperations();
void installCustomEntities();

/// \brief Base class for factories to create Operation instances
class OpFactoryBase {
  public:
    virtual ~OpFactoryBase();

    /// \brief Create a new operation using this factory on the heap
    virtual Operation * newOperation() = 0;
    /// \brief Create a new operation using this factory
    ///
    /// @param o Operation structure used to store the new operation
    virtual void newOperation(Operation & o) = 0;
};

/// \brief Class template for factories to create Operation instances of
/// the given Operation class.
template <class OpClass>
class OpFactory : public OpFactoryBase {
  public:
    virtual Operation * newOperation();
    virtual void newOperation(Operation &);
};

/// \brief Class for factories to create Operation instance with no hard coded
/// class using the Generic Operation class.
class GenericOpFactory : public OpFactoryBase {
  private:
    std::string m_opType;
  public:
    explicit GenericOpFactory(const std::string & opType);

    virtual Operation * newOperation();
    virtual void newOperation(Operation &);
};

typedef std::map<std::string, OpFactoryBase *> OpFactoryDict;
typedef std::map<std::string, Atlas::Objects::Root *> RootDict;

/// \brief Class to manage the inheritance tree for in-game entity types
class Inheritance {
  protected:
    RootDict atlasObjects;
    OpNoDict opLookup;
    OpFactoryDict opFactories;

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
    OpNo opEnumerate(const Operation &) const;
    Atlas::Objects::Root * get(const std::string & parent);
    int addChild(Atlas::Objects::Root * obj);
    Operation * newOperation(const std::string &);
    int newOperation(const std::string &, Operation &) const;
    bool isTypeOf(const std::string &, const std::string &) const;
};

#endif // COMMON_INHERITANCE_H
