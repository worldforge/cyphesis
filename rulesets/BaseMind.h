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

// $Id: BaseMind.h,v 1.49 2006-12-26 14:30:44 alriddoch Exp $

#ifndef RULESETS_BASE_MIND_H
#define RULESETS_BASE_MIND_H

#include "MemEntity.h"
#include "MemMap.h"

#include "modules/WorldTime.h"

/// \brief This is core class for representing the mind of an AI entity.
///
/// Instances of this class almost always have an associated script
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
class BaseMind : public MemEntity {
  protected:
    /// \brief Memory map of world entities this mind knows about
    MemMap m_map;
    /// \brief Flag indicating whether this mind is concious
    bool m_isAwake;
    /// \brief World time as far as this mind is aware
    WorldTime m_time;
    /// FIXME #5 Remove m_world, and the m_world in Entity, as we now
    /// use the singleton accessor in BaseWorld for that
    /// \brief Override of Entity::m_world to make sure we don't use it
    int m_world;
  public:
    BaseMind(const std::string &, long, const std::string &);
    virtual ~BaseMind();

    /// \brief Accessor for the memory map of world entities
    MemMap * getMap() { return &m_map; }
    /// \brief Accessor for the world time
    WorldTime * getTime() { return &m_time; }

    /// \brief Set this mind as inactive
    void sleep() { m_isAwake = false; }
    /// \brief Set this mind as active
    void awake() { m_isAwake = true; }

    void sightCreateOperation(const Operation &, const Operation &, OpVector &);
    void sightDeleteOperation(const Operation &, const Operation &, OpVector &);
    void sightMoveOperation(const Operation &, const Operation &, OpVector &);
    void sightSetOperation(const Operation &, const Operation &, OpVector &);

    virtual void scriptSubscribe(const std::string &);

    virtual void operation(const Operation &, OpVector &);

    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);

    void callSightOperation(const Operation &, const Operation &, OpVector &);
    void callSoundOperation(const Operation &, const Operation &, OpVector &);

    friend class PythonMindScript;
};

#endif // RULESETS_BASE_MIND_H
