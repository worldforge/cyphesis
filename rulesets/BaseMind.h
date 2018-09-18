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

#include "MemMap.h"

#include "modules/WorldTime.h"

/// \brief This is core class for representing the mind of an AI entity.
///
/// Instances of this class almost always have an associated script which
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
class BaseMind : public Router, public ReferenceCounted {
  protected:

    std::string m_entityId;

    Flags m_flags;

    std::unique_ptr<TypeResolver> m_typeResolver;

    /// \brief Memory map of world entities this mind knows about
    MemMap m_map;
    /// \brief World time as far as this mind is aware
    WorldTime m_time;

    std::map<std::string, std::vector<Operation>> m_pendingEntitiesOperations;
    std::vector<Operation> m_pendingOperations;



    std::map<long, std::function<void(const Operation &, OpVector &)>> m_callbacks;

    long m_serialNoCounter;

    std::unique_ptr<Script> m_script;

    Ref<MemEntity> m_ownEntity;


    public:
    BaseMind(const std::string & mindId, const std::string & entityId);

    ~BaseMind() override;

    void init(OpVector& res);

    /// \brief Accessor for the memory map of world entities
    MemMap * getMap() { return &m_map; }
    /// \brief Accessor for the world time
    WorldTime * getTime() { return &m_time; }

    /// \brief Is this mind active
    bool isAwake() const { return !m_flags.hasFlags(entity_asleep); }

    /// \brief Set this mind as inactive
    void sleep() { m_flags.addFlags(entity_asleep); }
    /// \brief Set this mind as active
    void awake() { m_flags.removeFlags(entity_asleep); }

    void sightCreateOperation(const Operation &, OpVector &);
    void sightDeleteOperation(const Operation &, OpVector &);
    void sightMoveOperation(const Operation &, OpVector &);
    void sightSetOperation(const Operation &, OpVector &);

    void operation(const Operation &, OpVector &) override;

    void externalOperation(const Operation & op, Link &) override;

    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);
    virtual void UnseenOperation(const Operation &, OpVector &);
    virtual void ThinkOperation(const Operation &, OpVector &);
    void InfoOperation(const Operation&, OpVector& );

    void callSightOperation(const Operation &, OpVector &);
    void callSoundOperation(const Operation &, OpVector &);

    friend class BaseMindMapEntityintegration;

    void setScript(Script * scrpt);

    /// \brief Check if this entity is flagged as destroyed
    bool isDestroyed() const { return m_flags.hasFlags(entity_destroyed); }

    virtual void setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity);

};

#endif // RULESETS_BASE_MIND_H
