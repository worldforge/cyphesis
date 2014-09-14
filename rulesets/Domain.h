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

#include <string>

class LocatedEntity;

/// \brief Base class for movement domains
///
/// The movement domain implements movement in the game world, including
/// visibility calculations, collision detection and physics.
/// Motion objects interact with the movement domain.
class Domain {
  private:

    /**
     * @brief The entity to which this domain belongs.
     */
    LocatedEntity& m_entity;

    /// Count of references held by other objects to this domain
    int m_refCount;

  public:
    Domain(LocatedEntity& entity);

    virtual ~Domain();

    /// \brief Increment the reference count on this domain
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this domain
    void decRef() {
        if (--m_refCount <= 0) {
            delete this;
        }
    }

    virtual float constrainHeight(LocatedEntity *, const Point3D &,
                                  const std::string &);

    virtual void tick(double t);

    /**
     * @brief Lets one entity look at another, calculating whether this can be done, and what children also can be seen.
     *
     * @param observingEntity The observer entity.
     * @param observedEntity The entity being looked at.
     * @param originalLookOp The originating Look operation.
     * @param res
     */
    void lookAtEntity(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity, const Operation & originalLookOp, OpVector& res) const;

    /**
     * @brief Checks if the observing Entity can see the observed entity.
     *
     * This is done by using both a distance check as well as an outfit and wielded check.
     *
     * @param observingEntity The observer entity.
     * @param observedEntity The entity being looked at.
     * @return True if the observer entity can see the observed entity.
     */
    bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const;
};

#endif // RULESETS_DOMAIN_H
