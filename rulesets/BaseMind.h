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
/// Instances of this class almost always have an associated script
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
class BaseMind : public MemEntity {
  protected:
    MemMap m_map;
    bool m_isAwake;
    WorldTime m_time;
    int m_world;
  public:
    BaseMind(const std::string &, long, const std::string &);
    virtual ~BaseMind();

    MemMap * getMap() { return &m_map; }
    WorldTime * getTime() { return &m_time; }

    void sleep() { m_isAwake = false; }
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
