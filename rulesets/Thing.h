// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_THING_H
#define RULESETS_THING_H

#include "Entity.h"

#include "common/BaseWorld.h"

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

    /// \brief Send an operation to the world for dispatch.
    ///
    /// sendWorld() bipasses serialno assignment, so you must ensure
    /// that serialno is sorted. This allows client serialnos to get
    /// in, so that client gets correct usefull refnos back.
    OpVector sendWorld(RootOperation * op) const {
        m_world->message(*op, this);
        return OpVector();
    }

    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector BurnOperation(const Burn & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector SetOperation(const Set & op);
};

#endif // RULESETS_THING_H
