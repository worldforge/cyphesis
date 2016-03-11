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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "VoidDomain.h"

#include "common/const.h"

VoidDomain::VoidDomain(LocatedEntity& entity)
: Domain(entity)
{
}

VoidDomain::~VoidDomain()
{
}

float VoidDomain::constrainHeight(LocatedEntity &, LocatedEntity *,
                              const Point3D &,
                              const std::string &)
{
    //Nothing can move
    return 0.0f;
}

double VoidDomain::tick(double t)
{
    return .0;
}

bool VoidDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    //Nothing can be seen
    return false;
}

void VoidDomain::processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) {
    //Nothing can be seen
}

void VoidDomain::processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) {
    //Nothing can be seen
}


float VoidDomain::checkCollision(LocatedEntity& entity, CollisionData& collisionData)
{
    //Nothing can move
    return consts::move_tick;
}

void VoidDomain::addEntity(LocatedEntity& entity) {

}
void VoidDomain::removeEntity(LocatedEntity& entity) {

}

