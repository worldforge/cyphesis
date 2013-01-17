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

// $Id$

#ifndef RULESETS_ENTITY_H
#define RULESETS_ENTITY_H

#include "LocatedEntity.h"

#include <iostream>

class Motion;

typedef std::map<int, std::string> DelegateMap;

/// \brief This is the base class from which all in-game objects inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes which are common to most
/// in game objects, the dynamic attributes map, and a means to access both
/// transparantly without needing to know which are which.
/// This is now also intended to be the base for in-game persistance.
/// It implements the basic types required for persistance.
/// \ingroup EntityClasses
class Entity : public LocatedEntity {
  protected:
    /// Motion behavoir of this entity
    Motion * m_motion;
    /// Map of operation handlers
    HandlerMap m_operationHandlers;
    /// Map of delegate properties.
    DelegateMap m_delegates;

  public:
    explicit Entity(const std::string & id, long intId);
    virtual ~Entity();

    /// \brief Accessor for pointer to motion object
    Motion * motion() const {
        return m_motion;
    }

    virtual PropertyBase * setAttr(const std::string & name,
                                   const Atlas::Message::Element &);
    virtual const PropertyBase * getProperty(const std::string & name) const;

    virtual PropertyBase * modProperty(const std::string & name);
    virtual PropertyBase * setProperty(const std::string & name, PropertyBase * prop);

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void ActuateOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void AttackOperation(const Operation &, OpVector &);
    virtual void CombineOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void DeleteOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);
    virtual void DivideOperation(const Operation &, OpVector &);
    virtual void EatOperation(const Operation &, OpVector &);
    virtual void GetOperation(const Operation &, OpVector &);
    virtual void ImaginaryOperation(const Operation &, OpVector &);
    virtual void InfoOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);
    virtual void MoveOperation(const Operation &, OpVector &);
    virtual void NourishOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void TalkOperation(const Operation &, OpVector &);
    virtual void TickOperation(const Operation &, OpVector &);
    virtual void TouchOperation(const Operation &, OpVector &);
    virtual void UpdateOperation(const Operation &, OpVector &);
    virtual void UseOperation(const Operation &, OpVector &);
    virtual void WieldOperation(const Operation &, OpVector &);

    virtual void externalOperation(const Operation & op, Link &);
    virtual void operation(const Operation &, OpVector &);

    void callOperation(const Operation &, OpVector &);

    virtual void installHandler(int, Handler);
    virtual void installDelegate(int, const std::string &);

    virtual void onContainered();
    virtual void onUpdated();

    virtual void destroy();

    virtual Domain * getMovementDomain();

    virtual void sendWorld(const Operation & op);

    friend class Entitytest;
    friend class PropertyEntityintegration;
};

inline std::ostream & operator<<(std::ostream& s, Location& v)
{
    s << "{";
    if (v.m_loc != 0) {
        s << v.m_loc->getId();
    } else {
        s << "null";
    }
    if (v.pos().isValid()) {
        s << "," << v.pos();
        if (v.velocity().isValid()) {
            s << "," << v.velocity();
        }
    }
    return s << "}";
}

#endif // RULESETS_ENTITY_H
