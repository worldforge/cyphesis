/*
 Copyright (C) 2015 Erik Ogenvik

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
#ifndef TESTS_TESTDOMAIN_H_
#define TESTS_TESTDOMAIN_H_

#include "rules/Domain.h"
#include "rules/LocatedEntity.h"
#include "common/const.h"

class TestDomain : public Domain
{
    public:

        TestDomain(LocatedEntity& entity) :
                Domain(entity)
        {
        }

        ~TestDomain()
        {
        }

        void tick(double t, OpVector& res)
        {
        }

        bool isEntityVisibleFor(
                const LocatedEntity& observingEntity,
                const LocatedEntity& observedEntity) const
        {
            return true;
        }

        virtual void getVisibleEntitiesFor(const LocatedEntity& observingEntity,
                                           std::list<LocatedEntity*>& entityList) const
        {

        }

        void addEntity(LocatedEntity& entity)
        {

        }

        void removeEntity(LocatedEntity& entity)
        {

        }

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override
        {
            return boost::none;
        }


};

#endif /* TESTS_TESTDOMAIN_H_ */
