// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef RULESETS_DOMAIN_H
#define RULESETS_DOMAIN_H

#include "physics/Vector3D.h"
#include "common/OperationRouter.h"

#include <wfmath/vector.h>

#include <string>

class LocatedEntity;
class Location;

/// \brief Base class for movement domains
///
/// The movement domain implements movement in the game world, including
/// visibility calculations, collision detection and physics.
/// Motion objects interact with the movement domain.
class Domain {
  protected:

    /**
     * @brief The entity to which this domain belongs.
     */
    LocatedEntity& m_entity;

  public:

    /**
     * @brief Collision data used when checking for collision.
     */
    struct CollisionData {
        ///True if there's a collision.
        bool isCollision;
        /// Entity with which collision will occur
        LocatedEntity * collEntity;
        /// Normal to the collision surface
        Vector3D collNormal;
    };

    Domain(LocatedEntity& entity);

    virtual ~Domain();

    virtual float constrainHeight(LocatedEntity *, const Point3D &,
                                  const std::string &) = 0;

    virtual void tick(double t) = 0;

    /**
     * @brief Lets one entity look at another, calculating whether this can be done, and what children also can be seen.
     *
     * @param observingEntity The observer entity.
     * @param observedEntity The entity being looked at.
     * @param originalLookOp The originating Look operation.
     * @param res
     */
    virtual void lookAtEntity(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity, const Operation & originalLookOp, OpVector& res) const = 0;

    /**
     * @brief Checks if the observing Entity can see the observed entity.
     *
     * This is done by using both a distance check as well as an outfit and wielded check.
     *
     * @param observingEntity The observer entity.
     * @param observedEntity The entity being looked at.
     * @return True if the observer entity can see the observed entity.
     */
    virtual bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const = 0;

    /**
     * @brief Process visibility operation for an entity that has been moved.
     *
     * This mainly involves calculating visibility changes, generating Appear and Disappear ops.
     *
     * @param moved_entity The entity that was moved.
     * @param old_loc The old location of the entity.
     * @param res
     */
    virtual void processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) = 0;

    /**
     * @brief Process an entity being moved out of the domain, and thus disappearing.
     * @param moved_entity
     * @param old_loc
     * @param res
     */
    virtual void processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) = 0;

    /**
     * Checks any upcoming collisions for the supplied entity.
     * @param entity The entity which is moving.
     * @param collisionData Collision data, to be populated.
     * @return Seconds until either a collision will occur, or we should check for collisions again.
     */
    virtual float checkCollision(LocatedEntity& entity, CollisionData& collisionData) = 0;

};

#endif // RULESETS_DOMAIN_H
