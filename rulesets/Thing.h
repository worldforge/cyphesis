// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_THING_H
#define RULESETS_THING_H

#include "Entity.h"

/// \brief This is the base class from which all physical or tangible in-game
/// entities inherit.
///
/// It can be used itself to represent any in-game object for which
/// any special behavior can be described by a script. It provides
/// functionality for movement, entity creation and destruction, attribute
/// changing, and combustion.
class Thing : public Entity {
  protected:
    void updateProperties(const Operation & op, OpVector & res);

    bool lookAtEntity(const Operation & op, OpVector & res, const LocatedEntity* watcher) const;
    void generateSightOp(const LocatedEntity& observingEntity, const Operation & originalLookOp, OpVector& res) const;

  public:

    explicit Thing(const std::string & id, long intId);
    virtual ~Thing();

    void DeleteOperation(const Operation & op, OpVector &) override;
    void MoveOperation(const Operation & op, OpVector &) override;
    void SetOperation(const Operation & op, OpVector &) override;
    void UpdateOperation(const Operation & op, OpVector &) override;
    void LookOperation(const Operation & op, OpVector &) override;
    void CreateOperation(const Operation & op, OpVector &) override;

    friend class ThingupdatePropertiestest;
};

#endif // RULESETS_THING_H
