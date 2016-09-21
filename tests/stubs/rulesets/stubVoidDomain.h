/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef STUBVOIDDOMAIN_H_
#define STUBVOIDDOMAIN_H_

VoidDomain::VoidDomain(LocatedEntity& entity)
: Domain(entity)
{
}

VoidDomain::~VoidDomain()
{
}

float VoidDomain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.0f;
}

void VoidDomain::tick(double t)
{

}

void VoidDomain::lookAtEntity(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity, const Operation & originalLookOp, OpVector& res) const
{
}

bool VoidDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    return false;
}

void VoidDomain::processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) {
}

float VoidDomain::checkCollision(LocatedEntity& entity, CollisionData& collisionData)
{
    return consts::move_tick;
}



#endif /* STUBVOIDDOMAIN_H_ */
