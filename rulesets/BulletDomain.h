// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_BULLET_DOMAIN_H
#define RULESETS_BULLET_DOMAIN_H

#include "rulesets/Domain.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btCollisionWorld;

/// \brief Movement domain using the bullet physics library
///
/// The movement domain implements movement in the game world, including
/// visibility calculations, collision detection and physics.
/// Motion objects interact with the movement domain.
class BulletDomain : public Domain {
  protected:
    btDefaultCollisionConfiguration * m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache;
    btCollisionWorld * m_collisionWorld;
  public:
    BulletDomain();

    virtual ~BulletDomain();

    virtual float constrainHeight(LocatedEntity *, const Point3D &,
                                  const std::string &);

    virtual void tick(double t);
};

#endif // RULESETS_BULLET_DOMAIN_H
