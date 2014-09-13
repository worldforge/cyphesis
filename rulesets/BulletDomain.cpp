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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "BulletDomain.h"

#include "common/debug.h"

#ifdef HAVE_BULLET
#include "btBulletCollisionCommon.h"
#endif // HAVE_BULLET

#include <cassert>

static const bool debug_flag = false;

BulletDomain::BulletDomain(LocatedEntity& entity) :
        Domain(entity),
#ifdef HAVE_BULLET
    // collision configuration contains default setup for memory,
    // collision setup. Advanced users can create their own configuration.
    m_collisionConfiguration(new btDefaultCollisionConfiguration()),
    // use the default collision dispatcher. For parallel processing you can
    // use a diffent dispatcher (see Extras/BulletMultiThreaded)
    m_dispatcher(new btCollisionDispatcher(m_collisionConfiguration)),
    // btDbvtBroadphase is a good general purpose broadphase.
    // You can also try out btAxis3Sweep.
    m_overlappingPairCache(new btAxisSweep3(btVector3(-1000,-1000,-1000),
                                            btVector3(1000,1000,1000))),
    // the default constraint solver. For parallel processing you can use a
    // different solver (see Extras/BulletMultiThreaded)
    // No need for constraint solver without dynamics
    // btSequentialImpulseConstraintSolver* solver =
    //       new btSequentialImpulseConstraintSolver;
    m_collisionWorld(new btCollisionWorld(m_dispatcher,
                                          m_overlappingPairCache,
                                          m_collisionConfiguration))
#else // HAVE_BULLET
    m_collisionConfiguration(0),
    m_dispatcher(0),
    m_overlappingPairCache(0),
    m_collisionWorld(0)
#endif // HAVE_BULLET
{
    // No gravity in collision world
    // collisionWorld->setGravity(btVector3(0,-10,0));
}

BulletDomain::~BulletDomain()
{
}

float BulletDomain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return Domain::constrainHeight(parent, pos, mode);
}

void BulletDomain::tick(double t)
{
    
}
