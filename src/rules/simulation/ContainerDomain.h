/*
 Copyright (C) 2019 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_CONTAINERDOMAIN_H
#define CYPHESIS_CONTAINERDOMAIN_H

#include "rules/Domain.h"
#include "ContainerAccessProperty.h"
#include <set>
#include <string>

class ContainerDomain : public Domain
{
    public:
        struct ObservationEntry;
        struct ClosenessObserverEntry
        {
            std::string reacherEntityId;
            LocatedEntity& target;
            /**
             * Callback to call when entries no longer are close.
             */
            std::function<void()> callback;
        };

        struct ObservationEntry
        {
            Ref<LocatedEntity> observer;

            /**
             * A list of disconnect functions which are to be called when the observation needs to be severed.
             */
            std::vector<std::function<void()>> disconnectFunctions;
            std::list<LocatedEntity*> observedEntities;
            std::set<ClosenessObserverEntry*> closenessObservations;
        };

        std::map<ClosenessObserverEntry*, std::unique_ptr<ClosenessObserverEntry>> m_closenessObservations;


        explicit ContainerDomain(LocatedEntity& entity);

        ~ContainerDomain() override = default;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::vector<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

        std::vector<CollisionEntry> queryCollision(const WFMath::Ball<3>& sphere) const override;

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override;


        LocatedEntity& getEntity()
        {
            return m_entity;
        }

        const std::map<std::string, ObservationEntry>& getEntries() const
        {
            return m_reachingEntities;
        };

        std::map<std::string, ObservationEntry>& getEntries()
        {
            return m_reachingEntities;
        };

        void setObservers(std::vector<std::string> observerIds);

        bool hasObserverRegistered(const LocatedEntity& entity) const;

        void removed() override;

        void addObserver(std::string& entityId);

        void removeObserver(const std::basic_string<char>& entityId);

    private:

        ContainerAccessProperty* mContainerAccessProperty;

        /**
         * A map of the entities that currently are reaching into this container.
         * Key is the entity id of the reaching entity.
         */
        std::map<std::string, ObservationEntry> m_reachingEntities;

};


#endif //CYPHESIS_CONTAINERDOMAIN_H
