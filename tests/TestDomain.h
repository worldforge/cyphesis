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

#include "rulesets/Domain.h"
#include "rulesets/LocatedEntity.h"
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

        float constrainHeight(LocatedEntity& entity, LocatedEntity * parent,
                const Point3D & pos, const std::string & mode)
        {
            return 0.0f;
        }

        double tick(double t)
        {
            return .0;
        }

        bool isEntityVisibleFor(
                const LocatedEntity& observingEntity,
                const LocatedEntity& observedEntity) const
        {
            return true;
        }

        void processVisibilityForMovedEntity(
                const LocatedEntity& moved_entity, const Location& old_loc,
                OpVector & res)
        {
        }

        void processDisappearanceOfEntity(
                const LocatedEntity& moved_entity, const Location& old_loc,
                OpVector & res)
        {
        }

        float checkCollision(LocatedEntity& entity,
                CollisionData& collisionData)
        {
            return consts::move_tick;
        }

        void addEntity(LocatedEntity& entity) {

        }
        void removeEntity(LocatedEntity& entity) {

        }
};

#endif /* TESTS_TESTDOMAIN_H_ */
