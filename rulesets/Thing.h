// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_THING_H
#define RULESETS_THING_H

#include "Entity.h"

typedef Entity Thing_parent;

/// \brief This is the base class from which all physical or tangiable in-game
/// entities inherit.
///
/// It can be used itself to represent any in-game object for which
/// any special behavior can be described by a script. It provides
/// functionality for movement, entity creation and destruction, attribute
/// changing, and combustion.
class Thing : public Thing_parent {
  public:

    explicit Thing(const std::string & id);
    virtual ~Thing();

    virtual void SetupOperation(const RootOperation & op, OpVector &);
    virtual void ActionOperation(const RootOperation & op, OpVector &);
    virtual void CreateOperation(const RootOperation & op, OpVector &);
    virtual void DeleteOperation(const RootOperation & op, OpVector &);
    virtual void BurnOperation(const RootOperation & op, OpVector &);
    virtual void MoveOperation(const RootOperation & op, OpVector &);
    virtual void SetOperation(const RootOperation & op, OpVector &);
};

#endif // RULESETS_THING_H
