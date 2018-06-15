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

#include "Domain.h"
#include <set>
#include <string>

/**
 * @brief An empty domain, in which nothing can move, and nothing can be seen.
 */
class InventoryDomain : public Domain
{
    public:
        explicit InventoryDomain(LocatedEntity& entity);

        ~InventoryDomain() override = default;

        void tick(double t, OpVector& res) override;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::list<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

};

#endif /* INVENTORYDOMAIN_H_ */
