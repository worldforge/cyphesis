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

#include "common/ScriptKit.h"

#include <deque>

class SimpleTypeStore;

struct TypeStore;

/// \brief This is core class for representing the mind of an AI entity.
///
/// Instances of this class almost always have an associated script which
/// handles most of the AI. The main purpose of this class is to handle
/// operations and interface to the MemMap used as the core of
/// the entity's memory.
class BaseMind : public Router, public ReferenceCounted, public MemMap::MapListener
{
    protected:

        std::string m_entityId;

        Flags m_flags;

        TypeStore& m_typeStore;
        std::unique_ptr<TypeResolver> m_typeResolver;

        /// \brief Memory map of world entities this mind knows about
        MemMap m_map;
        /// \brief World time as far as this mind is aware
        double mServerTime;

        std::map<std::string, std::vector<Operation>> m_pendingEntitiesOperations;
        std::deque<Operation> m_pendingOperations;

        long m_serialNoCounter;

        std::unique_ptr<Script> m_script;

        Ref<MemEntity> m_ownEntity;

        std::map<std::string, std::string> m_propertyScriptCallbacks;

        std::string m_addHook;
        std::string m_updateHook;
        std::string m_deleteHook;

        void removeEntity(const std::string& id, OpVector& res);

        void entityAdded(MemEntity& entity) override;

        void entityUpdated(MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation) override;

        void entityDeleted(MemEntity& entity) override;

        void updateServerTimeFromOperation(const Atlas::Objects::Operation::RootOperationData& op);

        /**
         * A list of outgoing operations as result from script hooks and callbacks.
         * These should be at every tick.
         */
        std::deque<Operation> mOutgoingOperations;

        struct TimeControl
        {
            std::chrono::steady_clock::time_point next;
            std::chrono::steady_clock::duration interval;
        };

        struct
        {
            TimeControl think;
            TimeControl move;
            TimeControl navmesh;
        } m_tickControl;


        virtual void processMove(OpVector& res)
        {};

        virtual void processNavmesh()
        {};
    public:
        BaseMind(RouterId mindId, std::string entityId, TypeStore& typeStore);

        ~BaseMind() override;

        ScriptKit<BaseMind>* m_scriptFactory;

        void init(OpVector& res);

        /**
         * Call this when the mind is destroyed and shouldn't receive any more ops.
         * This will clear any references to it.
         */
        void destroy();

        /// \brief Accessor for the memory map of world entities
        MemMap* getMap()
        { return &m_map; }

//        /// \brief Accessor for the world time
//        std::shared_ptr<WorldTime> getTime() const
//        { return m_time; }

        const Ref<MemEntity>& getEntity() const
        {
            return m_ownEntity;
        }

        const TypeStore& getTypeStore() const;

        /// \brief Is this mind active
        bool isAwake() const
        { return !m_flags.hasFlags(entity_asleep); }

        /// \brief Set this mind as inactive
        void sleep()
        { m_flags.addFlags(entity_asleep); }

        /// \brief Set this mind as active
        void awake()
        { m_flags.removeFlags(entity_asleep); }

        void sightSetOperation(const Operation&, OpVector&);

        void addPropertyScriptCallback(std::string propertyName, std::string scriptMethod);

        void operation(const Operation&, OpVector&) override;

        void externalOperation(const Operation& op, Link&) override;

        void SightOperation(const Operation&, OpVector&);

        void SoundOperation(const Operation&, OpVector&);

        void AppearanceOperation(const Operation&, OpVector&);

        void DisappearanceOperation(const Operation&, OpVector&);

        void UnseenOperation(const Operation&, OpVector&);

        void ThinkOperation(const Operation&, OpVector&);

        void InfoOperation(const Operation&, OpVector&);

        void ErrorOperation(const Operation& op, OpVector& res);

        void callSightOperation(const Operation&, OpVector&);

        void callSoundOperation(const Operation&, OpVector&);

        friend class BaseMindMapEntityintegration;

        void setScript(std::unique_ptr<Script> scrpt);

        /// \brief Check if this entity is flagged as destroyed
        bool isDestroyed() const
        { return m_flags.hasFlags(entity_destroyed); }

        virtual void setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity);

        std::string describeEntity() const;

        friend std::ostream& operator<<(std::ostream& s, const BaseMind& d);

        void setAddHook(std::string hook)
        { m_addHook = std::move(hook); }

        void setUpdateHook(std::string hook)
        { m_updateHook = std::move(hook); }

        void setDeleteHook(std::string hook)
        { m_deleteHook = std::move(hook); }

        void processTick(OpVector& res);

};

std::ostream& operator<<(std::ostream& s, const BaseMind& d);

#endif // RULESETS_BASE_MIND_H
