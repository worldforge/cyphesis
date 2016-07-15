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
#ifndef PHYSICALDOMAIN_H_
#define PHYSICALDOMAIN_H_

#include "Domain.h"

#include <map>
#include <unordered_map>
#include <array>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btCollisionWorld;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;
class btVector3;

/**
 * @brief A regular physical domain, behaving very much like the real world.
 *
 * Things move using physical rules, and sights are calculated using line of sight.
 *
 */
class PhysicalDomain: public Domain
{
    public:
        PhysicalDomain(LocatedEntity& entity);
        virtual ~PhysicalDomain();

        virtual float constrainHeight(LocatedEntity& entity, LocatedEntity *, const Point3D &, const std::string &);

        virtual double tick(double t);

        virtual bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const;

        virtual void processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res);

        virtual void processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res);

        virtual float checkCollision(LocatedEntity& entity, CollisionData& collisionData);

        virtual void addEntity(LocatedEntity& entity);
        virtual void removeEntity(LocatedEntity& entity);

        virtual void applyTransform(LocatedEntity& entity, const WFMath::Quaternion& orientation, const WFMath::Point<3>& pos, const WFMath::Vector<3>& velocity,
                const WFMath::AxisBox<3>& bbox);
        virtual void setVelocity(LocatedEntity& entity, const WFMath::Vector<3>& velocity);

    protected:

        struct BulletEntry
        {
                LocatedEntity* entity;
                btCollisionShape* collisionShape;
                btRigidBody* rigidBody;
        };

        class PhysicalMotionState;

        std::unordered_map<int, BulletEntry> m_entries;

        /**
         * @brief A map of all entities that currently are self-propelling.
         *
         * Each tick the propel force will be applied to these entities.
         */
        std::map<int, std::pair<BulletEntry*, btVector3>> m_propellingEntries;
        btDefaultCollisionConfiguration * m_collisionConfiguration;
        btCollisionDispatcher* m_dispatcher;
        btSequentialImpulseConstraintSolver* m_constraintSolver;
        btBroadphaseInterface* m_broadphase;
        btDiscreteDynamicsWorld * m_dynamicsWorld;

        int m_ticksPerSecond;

        std::unordered_map<std::string, std::array<float, 65 * 65>> m_terrainSegments;

        std::vector<btRigidBody*> m_borderPlanes;

        //btCollisionShape* m_groundCollisionShape;
        //btRigidBody* m_groundBody;

        /**
         * @brief Calculates visibility changes for the moved entity, processing the children of the "parent" parameter.
         * @param appear A list of appear ops, to be filled.
         * @param disappear A list of disappear ops, to be filled.
         * @param this_ent Atlas entity representing the entity that was moved.
         * @param parent The parent entity, which children will be iterated over.
         * @param moved_entity The entity that was moved.
         * @param old_loc The old location.
         * @param res
         */
        void calculateVisibility(std::vector<Atlas::Objects::Root>& appear, std::vector<Atlas::Objects::Root>& disappear, Atlas::Objects::Entity::Anonymous& this_ent,
                const LocatedEntity& parent, const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) const;

        /**
         * @brief Creates borders around the domain, which prevents entities from "escaping".
         *
         * This will be done by adding planes matching the bbox of the entity to which the domain belongs.
         */
        void createDomainBorders();

};

#endif /* PHYSICALDOMAIN_H_ */
