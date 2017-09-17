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


#ifndef RULESETS_BASE_MIND_H
#define RULESETS_BASE_MIND_H

#include "MemEntity.h"
#include "MemMap.h"

#include "modules/WorldTime.h"

/// \brief This is core class for representing the mind of an AI entity.
///
/// Instances of this class almost always have an associated script which
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
class BaseMind : public MemEntity {
  protected:
    /// \brief Memory map of world entities this mind knows about
    MemMap m_map;
    /// \brief World time as far as this mind is aware
    WorldTime m_time;
  public:
    BaseMind(const std::string & id, long intId);
    virtual ~BaseMind();

    /// \brief Accessor for the memory map of world entities
    MemMap * getMap() { return &m_map; }
    /// \brief Accessor for the world time
    WorldTime * getTime() { return &m_time; }

    /// \brief Is this mind active
    bool isAwake() const { return (getFlags() & entity_asleep) == 0; }

    /// \brief Set this mind as inactive
    void sleep() { setFlags(entity_asleep); }
    /// \brief Set this mind as active
    void awake() { resetFlags(entity_asleep); }

    void sightCreateOperation(const Operation &, OpVector &);
    void sightDeleteOperation(const Operation &, OpVector &);
    void sightMoveOperation(const Operation &, OpVector &);
    void sightSetOperation(const Operation &, OpVector &);

    virtual void thinkSetOperation(const Operation & op, OpVector & res);
    virtual void thinkDeleteOperation(const Operation & op, OpVector & res);
    virtual void thinkGetOperation(const Operation & op, OpVector & res);
    virtual void thinkLookOperation(const Operation & op, OpVector & res);

    void operation(const Operation &, OpVector &) override;

    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);
    virtual void UnseenOperation(const Operation &, OpVector &);
    virtual void ThinkOperation(const Operation &, OpVector &);

    void callSightOperation(const Operation &, OpVector &);
    void callSoundOperation(const Operation &, OpVector &);

    friend class BaseMindMapEntityintegration;
};

#endif // RULESETS_BASE_MIND_H
