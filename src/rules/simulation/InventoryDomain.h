/*
 Copyright (C) 2016 Erik Ogenvik

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
#ifndef INVENTORYDOMAIN_H_
#define INVENTORYDOMAIN_H_

#include "rules/Domain.h"
#include <set>
#include <string>
#include <map>

/**
 * @brief An inventory domain, typically attached to a character.
 * In this domain nothing can move. When viewed from the outside nothing can be seen, except for things that are attached to the entity.
 * Only the entity itself can reach things in the inventory.
 */
class InventoryDomain : public Domain
{
    public:
        explicit InventoryDomain(LocatedEntity& entity);

        ~InventoryDomain() override;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::vector<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

        std::vector<Domain::CollisionEntry> queryCollision(const WFMath::Ball<3>& sphere) const override;

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override;

    private:

        struct ClosenessObserverEntry
        {
            LocatedEntity& target;
            /**
             * Callback to call when entries no longer are close.
             */
            std::function<void()> callback;
        };

        std::map<std::string, std::map<ClosenessObserverEntry*, std::unique_ptr<ClosenessObserverEntry>>> m_closenessObservations;

        void removeClosenessObservation(const std::string& entityId);
//        std::map<ClosenessObserverEntry*, std::unique_ptr<ClosenessObserverEntry>> m_closenessObservations;

//        std::multimap<std::string, std::function<void()>> m_closenessObservations;
};

#endif /* INVENTORYDOMAIN_H_ */
