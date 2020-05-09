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
        explicit ContainerDomain(LocatedEntity& entity);

        ~ContainerDomain() override = default;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::list<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

        std::vector<CollisionEntry> queryCollision(const WFMath::Ball<3>& sphere) const override;

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override;

    private:

        ContainerAccessProperty& mContainerAccessProperty;

};


#endif //CYPHESIS_CONTAINERDOMAIN_H
