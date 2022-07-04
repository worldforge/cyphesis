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


#ifndef RULESETS_ENTITY_H
#define RULESETS_ENTITY_H

#include "rules/LocatedEntity.h"

#include <unordered_map>

struct OperationsListener;

/// \brief This is the base class from which all in-game objects inherit.
///
/// This class should not normally be instantiated directly.
/// This class provides hard-coded attributes which are common to most
/// in game objects, the dynamic attributes map, and a means to access both
/// transparently without needing to know which are which.
/// This is now also intended to be the base for in-game persistence.
/// It implements the basic types required for persistence.
/// \ingroup EntityClasses
class Entity : public LocatedEntity
{
    protected:

        /// Map of delegate properties.
        std::multimap<int, std::string> m_delegates;

        /// A static map tracking the number of existing entities per type.
        /// A monitor by the name of "entity_count{type=*}" will be created
        /// per type.
        static std::unordered_map<const TypeNode*, int> s_monitorsMap;

        std::unique_ptr<Domain> m_domain;

        std::unique_ptr<PropertyBase> createProperty(const std::string& propertyName) const override;

    public:
        explicit Entity(RouterId id);

        ~Entity() override;

        std::vector<OperationsListener*> m_listeners;

        void setType(const TypeNode* t) override;

        void addToMessage(Atlas::Message::MapType&) const override;

        void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override;

        virtual void DeleteOperation(const Operation&, OpVector&);

        virtual void ImaginaryOperation(const Operation&, OpVector&);

        virtual void LookOperation(const Operation&, OpVector&);

        virtual void MoveOperation(const Operation&, OpVector&);

        virtual void SetOperation(const Operation&, OpVector&);

        virtual void TalkOperation(const Operation&, OpVector&);

        virtual void UpdateOperation(const Operation&, OpVector&);

        virtual void RelayOperation(const Operation&, OpVector&);

        virtual void CreateOperation(const Operation& op, OpVector& res);

        void externalOperation(const Operation& op, Link&) override;

        void operation(const Operation&, OpVector&) override;

        HandlerResult callDelegate(const std::string&,
                                   const Operation&,
                                   OpVector&);

        /// \brief Find and call the handler for an operation
        ///
        /// @param op The operation to be processed.
        /// @param res The result of the operation is returned here.
        void callOperation(const Operation&, OpVector&);

        void installDelegate(int, const std::string&) override;

        void removeDelegate(int, const std::string&) override;

        void addChild(LocatedEntity& childEntity) override;

        void removeChild(LocatedEntity& childEntity) override;

        void onContainered(const Ref<LocatedEntity>& oldLocation) override;

        void onUpdated() override;

        void destroy() override;

        Domain* getDomain() override;

        const Domain* getDomain() const override;

        void setDomain(std::unique_ptr<Domain> domain) override;

        void sendWorld(Operation op) override;

        void addListener(OperationsListener* listener) override;

        void removeListener(OperationsListener* listener) override;

        Ref<LocatedEntity> createNewEntity(const Atlas::Objects::Entity::RootEntity& entity);
        Ref<LocatedEntity> createNewEntity(const Operation& op, OpVector& res);
};

#endif // RULESETS_ENTITY_H
