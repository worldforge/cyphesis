/*
 Copyright (C) 2017 Erik Ogenvik

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

#include "PhysicalWorld.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "Remotery.h"

PhysicalWorld::PhysicalWorld(btDispatcher* dispatcher,
                             btBroadphaseInterface* pairCache,
                             btConstraintSolver* constraintSolver,
                             btCollisionConfiguration* collisionConfiguration)
        : btDiscreteDynamicsWorld(dispatcher, pairCache, constraintSolver, collisionConfiguration)
{}

void PhysicalWorld::synchronizeMotionStates()
{
    //Don't do anything here
}

int PhysicalWorld::stepSimulation(btScalar timeStep, int maxSubSteps, btScalar fixedTimeStep)
{
    rmt_ScopedCPUSample(PhysicalWorld_stepSimulation, 0)

    int steps = btDiscreteDynamicsWorld::stepSimulation(timeStep, maxSubSteps, fixedTimeStep);

    rmt_ScopedCPUSample(PhysicalWorld_synchronizeMotionStates, 0)
    //iterate over all active rigid bodies
    for (int i = 0; i < m_nonStaticRigidBodies.size(); i++) {
        btRigidBody* body = m_nonStaticRigidBodies[i];
        if (body->isActive()) {
            synchronizeSingleMotionState(body);
        }
    }
    return steps;
}
