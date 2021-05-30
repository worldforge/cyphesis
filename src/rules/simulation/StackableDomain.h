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

#ifndef CYPHESIS_STACKABLEDOMAIN_H
#define CYPHESIS_STACKABLEDOMAIN_H

#include "rules/Domain.h"
#include <set>
#include <string>

/**
 * An entity with a StackableDomain is stackable.
 *
 * This works as such that when other entities that are of the same type are moved into them,
 * as children, they are removed, and the "amount" property of the stackable entity is increased.
 *
 * Splitting a stack is done by issuing a Move op with an extra "amount" attribute. This will create
 * a new entity, with the specified amount, while the existing entity will have its "amount" decreased.
 *
 * Deleting a stackable entity will decrease it's "amount" by one, or by a value specified in the "amount"
 * attribute in the Delete ops first arg.
 */
class StackableDomain : public Domain
{
    public:
        explicit StackableDomain(LocatedEntity& entity);

        ~StackableDomain() override = default;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::vector<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

        void installDelegates(LocatedEntity& entity, const std::string& propertyName) override;

        HandlerResult operation(LocatedEntity& entity, const Operation& op, OpVector& res) override;

        static bool checkEntitiesStackable(const LocatedEntity& first, const LocatedEntity& second);

        static bool stackIfPossible(const LocatedEntity& domainEntity, LocatedEntity& entity);

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override;

    protected:
        static std::vector<std::string> sIgnoredProps;

        HandlerResult DeleteOperation(LocatedEntity& owner, const Operation& op, OpVector& res);
        HandlerResult MoveOperation(LocatedEntity& owner, const Operation& op, OpVector& res);
};


#endif //CYPHESIS_STACKABLEDOMAIN_H
