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

#include "PhysicalDomain.h"

#include "TerrainProperty.h"
#include "LocatedEntity.h"
#include "OutfitProperty.h"
#include "EntityProperty.h"
#include "ModeProperty.h"
#include "PropelProperty.h"
#include "TransformsProperty.h"

#include "physics/Collision.h"
#include "physics/Convert.h"

#include "common/debug.h"
#include "common/const.h"
#include "common/Unseen.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <sigc++/bind.h>

#include <iostream>
#include <unordered_set>

#include <cassert>

static const bool debug_flag = true;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Move;

using Atlas::Objects::smart_dynamic_cast;

using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Operation::Unseen;

bool fuzzyEquals(float a, float b, float epsilon)
{
    return std::abs(a - b) < epsilon;
}

bool fuzzyEquals(const WFMath::Point<3>& a, const WFMath::Point<3>& b, float epsilon)
{
    return fuzzyEquals(a.x(), b.x(), epsilon) && fuzzyEquals(a.y(), b.y(), epsilon) && fuzzyEquals(a.z(), b.z(), epsilon);
}

bool fuzzyEquals(const WFMath::Vector<3>& a, const WFMath::Vector<3>& b, float epsilon)
{
    return fuzzyEquals(a.x(), b.x(), epsilon) && fuzzyEquals(a.y(), b.y(), epsilon) && fuzzyEquals(a.z(), b.z(), epsilon);
}

/**
 * Mask used by all physical items. They should collide with other physical items, and with the terrain.
 */
int COLLISION_MASK_PHYSICAL = 1;
/**
 * Mask used by the terrain. It's static.
 */
int COLLISION_MASK_NON_PHYSICAL = 2;
/**
 * Mask used by all non-physical items. These should only collide with the terrain.
 */
int COLLISION_MASK_TERRAIN = 4;

class PhysicalDomain::PhysicalMotionState: public btMotionState
{
    public:
        BulletEntry& m_bulletEntry;
        PhysicalDomain& m_domain;
        btTransform m_worldTrans;
        btTransform m_centerOfMassOffset;

        PhysicalMotionState(BulletEntry& bulletEntry, PhysicalDomain& domain, const btTransform& startTrans, const btTransform& centerOfMassOffset = btTransform::getIdentity()) :
                m_bulletEntry(bulletEntry), m_domain(domain), m_worldTrans(startTrans), m_centerOfMassOffset(centerOfMassOffset)

        {
        }

        ///synchronizes world transform from user to physics
        virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const
        {
//            debug_print("getWorldTransform: "<< m_entity.describeEntity());
            centerOfMassWorldTrans = m_worldTrans * m_centerOfMassOffset.inverse();
        }

        ///synchronizes world transform from physics to user
        ///Bullet only calls the update of worldtransform for active objects
        virtual void setWorldTransform(const btTransform& /* centerOfMassWorldTrans */)
        {

            LocatedEntity& entity = *m_bulletEntry.entity;
            m_domain.m_movingEntities.insert(&m_bulletEntry);

//            debug_print(
//                    "setWorldTransform: "<< m_entity.describeEntity() << " (" << centerOfMassWorldTrans.getOrigin().x() << "," << centerOfMassWorldTrans.getOrigin().y() << "," << centerOfMassWorldTrans.getOrigin().z() << ")");

            btTransform newTransform = m_bulletEntry.rigidBody->getCenterOfMassTransform() * m_centerOfMassOffset;

            entity.m_location.m_pos = Convert::toWF<WFMath::Point<3>>(newTransform.getOrigin());
            entity.m_location.m_orientation = Convert::toWF(newTransform.getRotation());
            entity.m_location.m_angularVelocity = Convert::toWF<WFMath::Vector<3>>(m_bulletEntry.rigidBody->getAngularVelocity());
            entity.m_location.m_velocity = Convert::toWF<WFMath::Vector<3>>(m_bulletEntry.rigidBody->getLinearVelocity());

            //If the magnitude is small enough, consider the velocity to be zero.
            if (entity.m_location.m_velocity.sqrMag() < 0.001f) {
                entity.m_location.m_velocity.zero();
            }
            if (entity.m_location.m_angularVelocity.sqrMag() < 0.001f) {
                entity.m_location.m_angularVelocity.zero();
            }
            entity.resetFlags(entity_pos_clean | entity_orient_clean);
            entity.setFlags(entity_dirty_location);

            TransformsProperty* transProp = entity.modPropertyClassFixed<TransformsProperty>();

            //Set here, but don't apply yet.
            transProp->getTranslate() = WFMath::Vector<3>(entity.m_location.m_pos);
            transProp->getRotate() = entity.m_location.m_orientation;

        }
};

PhysicalDomain::PhysicalDomain(LocatedEntity& entity) :
        Domain(entity),
        //default config for now
        m_collisionConfiguration(new btDefaultCollisionConfiguration()), m_dispatcher(new btCollisionDispatcher(m_collisionConfiguration)), m_constraintSolver(
                new btSequentialImpulseConstraintSolver()),
        //Use a dynamic broadphase; this might be worth revisiting for optimizations
        m_broadphase(new btDbvtBroadphase()), m_dynamicsWorld(new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration)), m_ticksPerSecond(
                15)
{

    createDomainBorders();

    //Update the linear velocity of all self propelling entities each tick.
    auto tickCallback = [](btDynamicsWorld *world, btScalar timeStep) {
        std::map<int, std::pair<BulletEntry*, btVector3>>* propellingEntries = static_cast<std::map<int, std::pair<BulletEntry*, btVector3>>*>(world->getWorldUserInfo());
        for (auto& entry : *propellingEntries) {
            float verticalVelocity = entry.second.first->rigidBody->getLinearVelocity().y();

            //Apply gravity
            if (verticalVelocity != 0) {
                verticalVelocity += world->getGravity().y() * timeStep;
                entry.second.first->rigidBody->setLinearVelocity(entry.second.second + btVector3(0, verticalVelocity, 0));
            } else {
                entry.second.first->rigidBody->setLinearVelocity(entry.second.second);

            }

            entry.second.first->rigidBody->activate();
        }
    };

    m_dynamicsWorld->setInternalTickCallback(tickCallback, &m_propellingEntries, true);

    if (true) {
        buildTerrainPages();
    } else {
        btStaticPlaneShape *plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
        btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion::getIdentity(), btVector3(0, 5, 0)));
        btRigidBody* planeBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, motionState, plane));
        //planeBody->setFriction(.0f);
        m_dynamicsWorld->addRigidBody(planeBody);
    }
//m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

PhysicalDomain::~PhysicalDomain()
{
    for (btRigidBody* planeBody : m_borderPlanes) {
        delete planeBody->getMotionState();
        delete planeBody->getCollisionShape();
        delete planeBody;
    }

    for (auto& entry : m_terrainSegments) {
        delete entry.second.data;
        delete entry.second.rigidBody->getMotionState();
        delete entry.second.rigidBody->getCollisionShape();
        delete entry.second.rigidBody;
    }

    for (auto& entry : m_entries) {
        if (entry.second->rigidBody) {
            m_dynamicsWorld->removeRigidBody(entry.second->rigidBody);
            delete entry.second->rigidBody->getMotionState();
            delete entry.second->rigidBody;
        }
        if (entry.second->collisionShape) {
            delete entry.second->collisionShape;
        }
        entry.second->propertyUpdatedConnection.disconnect();
        delete entry.second;

    }

    delete m_dynamicsWorld;
    delete m_broadphase;
    delete m_constraintSolver;
    delete m_dispatcher;
    delete m_collisionConfiguration;
    m_propertyAppliedConnection.disconnect();
}

void PhysicalDomain::buildTerrainPages()
{
    float friction = 1.0f;

    const Property<float>* frictionProp = m_entity.getPropertyType<float>("friction");

    if (frictionProp) {
        friction = frictionProp->data();
    }

    const TerrainProperty* terrainProperty = m_entity.getPropertyClass<TerrainProperty>("terrain");
    if (terrainProperty) {
        auto& terrain = terrainProperty->getData();
        auto segments = terrain.getTerrain();
        for (auto& row : segments) {
            for (auto& entry : row.second) {
                Mercator::Segment* segment = entry.second;
                buildTerrainPage(*segment, friction);
            }
        }
    }
}

void PhysicalDomain::buildTerrainPage(Mercator::Segment& segment, float friction)
{
    if (!segment.isValid()) {
        segment.populate();
    }

    int vertexCountOneSide = segment.getSize();

    std::stringstream ss;
    ss << segment.getXRef() << ":" << segment.getYRef();
    TerrainEntry& terrainEntry = m_terrainSegments[ss.str()];
    if (!terrainEntry.data) {
        terrainEntry.data = new std::array<float, 65 * 65>();
    }
    float* data = terrainEntry.data->data();
    const float* mercatorData = segment.getPoints();
    //Need to rotate to fit Bullet coord space.
    for (int y = 0; y < vertexCountOneSide; ++y) {
        for (int x = 0; x < vertexCountOneSide; ++x) {
            data[(vertexCountOneSide * (vertexCountOneSide - y - 1)) + x] = mercatorData[(vertexCountOneSide * y) + x];
        }
    }

    float min = segment.getMin();
    float max = segment.getMax();
    btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(vertexCountOneSide, vertexCountOneSide, data, 1.0f, min, max, 1, PHY_FLOAT, false);

    terrainShape->setLocalScaling(btVector3(1, 1, 1));

    float res = (float)segment.getResolution();

    float xPos = segment.getXRef() + (res / 2);
    float yPos = segment.getYRef() + (res / 2);
    float zPos = segment.getMin() + ((segment.getMax() - segment.getMin()) * 0.5f);

    WFMath::Point<3> pos(xPos, yPos, zPos);
    btVector3 btPos = Convert::toBullet(pos);

    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion::getIdentity(), btPos));
    btRigidBody::btRigidBodyConstructionInfo segmentCI(.0f, motionState, terrainShape);
    segmentCI.m_friction = friction;
    btRigidBody* segmentBody = new btRigidBody(segmentCI);

    m_dynamicsWorld->addRigidBody(segmentBody, COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN,
            COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN);

    terrainEntry.rigidBody = segmentBody;

}

void PhysicalDomain::createDomainBorders()
{
    auto& bbox = m_entity.m_location.bBox();
    if (bbox.isValid()) {
        //We'll now place six planes representing the bounding box.

        m_borderPlanes.reserve(6);
        auto createPlane =
                [&](const btVector3& normal, const btVector3& translate) {
                    btStaticPlaneShape *plane = new btStaticPlaneShape(normal, 0);
                    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion::getIdentity(), translate));
                    btRigidBody* planeBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, motionState, plane));
                    m_dynamicsWorld->addRigidBody(planeBody, COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN, COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN);
                    m_borderPlanes.push_back(planeBody);
                };

        //Bottom plane
        createPlane(btVector3(0, 1, 0), btVector3(0, bbox.lowerBound(2), 0));

        //Top plane
        createPlane(btVector3(0, -1, 0), btVector3(0, bbox.upperBound(2), 0));

        //Crate surrounding planes
        createPlane(btVector3(1, 0, 0), btVector3(bbox.lowerBound(0), 0, 0));
        createPlane(btVector3(-1, 0, 0), btVector3(bbox.upperBound(0), 0, 0));
        createPlane(btVector3(0, 0, 1), btVector3(0, 0, bbox.lowerBound(1)));
        createPlane(btVector3(0, 0, -1), btVector3(0, 0, bbox.upperBound(1)));
    }
}

bool PhysicalDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    if (&observedEntity == &m_entity) {
        return true;
    }

    //We need to check the distance to the entity being looked at, and make sure that both the looking entity and
    //the entity being looked at belong to the same domain
    const Location* ancestor;
    //We'll optimize for the case when a child entity is looking at the domain entity, by sending the looked at entity first, since this is the most common case.
    //The squareDistanceWithAncestor() method will first try to find the first entity being sent by walking upwards from the second entity being sent (if
    //it fails it will try other approaches).
    float distance = squareDistanceWithAncestor(observedEntity.m_location, observingEntity.m_location, &ancestor);
    if (ancestor == nullptr) {
        //No common ancestor found
        return false;
    } else {
        //Make sure that the ancestor is the domain entity, or a child entity.
        while (ancestor != &m_entity.m_location) {
            if (ancestor->m_loc == nullptr) {
                //We've reached the top of the parents chain without hitting our domain entity; the ancestor isn't a child of the domain entity.
                return false;
            }
            ancestor = &ancestor->m_loc->m_location;
        }
    }
    //If we get here we know that the ancestor is a child of the domain entity.
    //Now we need to determine if the looking entity can see the looked at entity. The default way of doing this is by comparing the size of the looked at entity with the distance,
    //but this check can be overridden if the looked at entity is either wielded or outfitted by a parent entity.
    if ((observedEntity.m_location.squareBoxSize() / distance) > consts::square_sight_factor) {
        return true;
    }
    return false;
}

void PhysicalDomain::calculateVisibility(std::vector<Root>& appear, std::vector<Root>& disappear, Anonymous& this_ent, const LocatedEntity& parent,
        const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res) const
{

    float fromSquSize = moved_entity.m_location.squareBoxSize();

    //We need to get the position of the moved entity in relation to the parent.
    const Point3D new_pos = relativePos(parent.m_location, moved_entity.m_location);
    const Point3D old_pos = relativePos(parent.m_location, old_loc);

    //For now we'll only consider movement within the same loc. This should change as we extend the domain code.
    assert(parent.m_contains != nullptr);
    for (const LocatedEntity* other : *parent.m_contains) {
        if (other == &moved_entity) {
            continue;
        }

        assert(other != nullptr);
        float old_dist = squareDistance(other->m_location.pos(), old_pos), new_dist = squareDistance(other->m_location.pos(), new_pos), squ_size =
                other->m_location.squareBoxSize();

        // Build appear and disappear lists, and send disappear operations
        // to perceptive entities saying that we are disappearing
        if (other->isPerceptive()) {
            bool was_in_range = ((fromSquSize / old_dist) > consts::square_sight_factor), is_in_range = ((fromSquSize / new_dist) > consts::square_sight_factor);
            if (was_in_range != is_in_range) {
                if (was_in_range) {
                    // Send operation to the entity in question so it
                    // knows it is losing sight of us.
                    Disappearance d;
                    d->setArgs1(this_ent);
                    d->setTo(other->getId());
                    res.push_back(d);
                }
                //Note that we don't send any Appear ops for those entities that we now move within sight range of.
                //This is because these will receive a Move op anyway as part of the broadcast, which informs them
                //that an entity has moved within sight range anyway.
            }
        }

        bool could_see = ((squ_size / old_dist) > consts::square_sight_factor), can_see = ((squ_size / new_dist) > consts::square_sight_factor);
        if (could_see ^ can_see) {
            Anonymous that_ent;
            that_ent->setId(other->getId());
            that_ent->setStamp(other->getSeq());
            if (could_see) {
                // We are losing sight of that object
                disappear.push_back(that_ent);
//                debug(std::cout << moved_entity.getId() << ": losing sight of " << other->getId() << std::endl
//                ;);
            } else /*if (can_see)*/{
                // We are gaining sight of that object
                appear.push_back(that_ent);
//                debug(std::cout << moved_entity.getId() << ": gaining sight of " << other->getId() << std::endl
//                ;);
            }
            //        } else {
            //            //We've seen this entity before, and we're still seeing it. Check if there are any children that's now changing visibility.
            //            if (other->m_contains && !other->m_contains->empty()) {
            //                calculateVisibility(appear, disappear, this_ent, *other, moved_entity, old_loc, res);
            //            }
        }
    }
}

void PhysicalDomain::processVisibilityForMovedEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res)
{
    debug_print("PhysicalDomain::processVisibilityForMovedEntity testing range for " << moved_entity.describeEntity());
    std::vector<Root> appear, disappear;

    Anonymous this_ent;
    this_ent->setId(moved_entity.getId());
    this_ent->setStamp(moved_entity.getSeq());

    calculateVisibility(appear, disappear, this_ent, m_entity, moved_entity, old_loc, res);

    if (!appear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are gaining sight of
        Appearance a;
        a->setArgs(appear);
        a->setTo(moved_entity.getId());
        res.push_back(a);
    }
    if (!disappear.empty()) {
        // Send an operation to ourselves with a list of entities
        // we are losing sight of
        Disappearance d;
        d->setArgs(disappear);
        d->setTo(moved_entity.getId());
        res.push_back(d);
    }
}

void PhysicalDomain::processDisappearanceOfEntity(const LocatedEntity& moved_entity, const Location& old_loc, OpVector & res)
{

    float fromSquSize = old_loc.squareBoxSize();
    Anonymous this_ent;
    this_ent->setId(moved_entity.getId());
    this_ent->setStamp(moved_entity.getSeq());

    //We need to get the position of the moved entity in relation to the parent.
    const Point3D old_pos = relativePos(m_entity.m_location, old_loc);

    assert(m_entity.m_contains != nullptr);
    for (const LocatedEntity* other : *m_entity.m_contains) {
        //No need to check if we iterate over ourselved; that won't happen if we've disappeared

        assert(other != nullptr);

        // Build appear and disappear lists, and send disappear operations
        // to perceptive entities saying that we are disappearing
        if (other->isPerceptive()) {
            float old_dist = squareDistance(other->m_location.pos(), old_pos);
            if ((fromSquSize / old_dist) > consts::square_sight_factor) {
                // Send operation to the entity in question so it
                // knows it is losing sight of us.
                Disappearance d;
                d->setArgs1(this_ent);
                d->setTo(other->getId());
                res.push_back(d);
            }
        }
    }
}

float PhysicalDomain::checkCollision(LocatedEntity& entity, CollisionData& collisionData)
{
    return 0;
}

float PhysicalDomain::getMassForEntity(const LocatedEntity& entity) const
{
    float mass = 0;

    if (entity.getType()->isTypeOf("creator")) {
        mass = 1.0f;
    }

    auto massProp = entity.getPropertyType<double>("mass");
    if (massProp) {
        mass = massProp->data();
    }
    return mass;
}

void PhysicalDomain::addEntity(LocatedEntity& entity)
{
    assert(m_entries.find(entity.getIntId()) == m_entries.end());

    float mass = getMassForEntity(entity);

    WFMath::AxisBox<3> bbox = entity.m_location.bBox();

    //Handle the special case of the entity being a "creator".
    if (entity.getType()->isTypeOf("creator") && !bbox.isValid()) {
        bbox = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25, -0.25, 0), WFMath::Point<3>(0.25, 0.25, 1.5));
    }

    BulletEntry* entry = new BulletEntry();
    entry->entity = &entity;

    btVector3 angularFactor(1, 1, 1);
    short collisionMask;
    short collisionGroup;
    getCollisionFlagsForEntity(entity, collisionGroup, collisionMask);

    //TODO: Use properties for geometry instead.
    if (entity.getType()->isTypeOf("mobile") || entity.getType()->isTypeOf("creator")) {
        float radius = (bbox.highCorner().x() - bbox.lowCorner().x()) * 0.5f;
        //subtract the radius times 2 from the height
        float height = bbox.highCorner().z() - bbox.lowCorner().z() - (radius * 2.0f);
        entry->collisionShape = new btCapsuleShape(radius, height);
        angularFactor = btVector3(0, 0, 0);
    } else {
        WFMath::Vector<3> size;
        if (bbox.isValid()) {
            size = bbox.highCorner() - bbox.lowCorner();
            size *= 0.5;
        } else {
            //No bbox, and no "creator" entity
            return;
        }

        auto btSize = Convert::toBullet(size).absolute();
        entry->collisionShape = new btBoxShape(btSize);
    }

    btVector3 inertia;
    entry->collisionShape->calculateLocalInertia(mass, inertia);

    std::string mode;
    auto modeProp = entity.getPropertyClassFixed<ModeProperty>();
    if (modeProp) {
        mode = modeProp->data();
    }

    auto adjustHeightFn = [&]() {
        const TerrainProperty * tp = m_entity.getPropertyClass<TerrainProperty>("terrain");
        if (tp) {
            TransformsProperty* transProp = entity.modPropertyClassFixed<TransformsProperty>();
            const WFMath::Point<3>& pos = entity.m_location.pos();

            float h = pos.z();
            Vector3D normal;
            tp->getHeightAndNormal(pos.x(), pos.y(), h, normal);
            transProp->getTranslate().z() = h;
            transProp->apply(&entity);
        }};

    if (mode != "fixed") {
        adjustHeightFn();
    }

    if (mode == "planted" || mode == "fixed") {
        //"fixed" mode means that the entity stays in place, always
        //"planted" mode means it's planted in the ground
        //Zero mass makes the rigid body static
        mass = .0f;
    }

    //"Center of mass offset" is the inverse of the center of the object in relation to origo.
    btVector3 centerOfMassOffset = -Convert::toBullet(bbox.getCenter());

    btQuaternion orientation = entity.m_location.m_orientation.isValid() ? Convert::toBullet(entity.m_location.m_orientation) : btQuaternion(0, 0, 0, 1);
    btVector3 pos = entity.m_location.m_pos.isValid() ? Convert::toBullet(entity.m_location.m_pos) : btVector3(0, 0, 0);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, nullptr, entry->collisionShape, inertia);

    const Property<float>* frictionProp = entity.getPropertyType<float>("friction");
    if (frictionProp) {
        rigidBodyCI.m_friction = frictionProp->data();
    }

    debug_print(
            "PhysicsDomain adding entity " << entity.describeEntity() << " with mass " << mass << " and inertia ("<< inertia.x() << ","<< inertia.y() << ","<< inertia.z() << ")");

    entry->rigidBody = new btRigidBody(rigidBodyCI);
    entry->rigidBody->setMotionState(new PhysicalMotionState(*entry, *this, btTransform(orientation, pos), btTransform(btQuaternion::getIdentity(), centerOfMassOffset)));
    entry->rigidBody->setAngularFactor(angularFactor);

    m_dynamicsWorld->addRigidBody(entry->rigidBody, collisionGroup, collisionMask);

    if (mass != 0) {
        //Should all entities be active when added?
        entry->rigidBody->activate();
    }

    entry->propertyUpdatedConnection = entity.propertyApplied.connect(sigc::bind(sigc::mem_fun(this, &PhysicalDomain::childEntityPropertyApplied), entry));

    m_entries.insert(std::make_pair(entity.getIntId(), entry));

    const PropelProperty* propelProp = entity.getPropertyClassFixed<PropelProperty>();
    if (propelProp && propelProp->data().isValid() && propelProp->data() != WFMath::Vector<3>::ZERO()) {
        btVector3 btVelocity = Convert::toBullet(propelProp->data());

        auto I = m_propellingEntries.find(entity.getIntId());
        if (I == m_propellingEntries.end()) {
            m_propellingEntries.insert(std::make_pair(entity.getIntId(), std::make_pair(entry, btVelocity)));
        } else {
            I->second.second = btVelocity;
        }

    }
}

void PhysicalDomain::removeEntity(LocatedEntity& entity)
{
    debug_print("PhysicalDomain::removeEntity " << entity.describeEntity());
    auto I = m_entries.find(entity.getIntId());
    assert(I != m_entries.end());
    if (I->second->rigidBody) {
        m_dynamicsWorld->removeRigidBody(I->second->rigidBody);
        delete I->second->rigidBody->getMotionState();
        delete I->second->rigidBody;
    }
    if (I->second->collisionShape) {
        delete I->second->collisionShape;
    }
    I->second->propertyUpdatedConnection.disconnect();
    delete I->second;
    m_entries.erase(I);

    m_propellingEntries.erase(entity.getIntId());
}

void PhysicalDomain::childEntityPropertyApplied(const std::string& name, PropertyBase& prop, BulletEntry* bulletEntry)
{
    if (name == "friction") {
        Property<float>* frictionProp = static_cast<Property<float>*>(&prop);
        bulletEntry->rigidBody->setFriction(frictionProp->data());
        bulletEntry->rigidBody->activate();
        return;
    } else if (name == "mode") {
        LocatedEntity& entity = *bulletEntry->entity;

        ModeProperty* modeProp = static_cast<ModeProperty*>(&prop);

        const std::string& mode = modeProp->data();

        if (mode != "fixed") {
            const TerrainProperty * tp = m_entity.getPropertyClass<TerrainProperty>("terrain");
            if (tp) {
                TransformsProperty* transProp = entity.modPropertyClassFixed<TransformsProperty>();
                const WFMath::Point<3>& wfPos = entity.m_location.pos();

                float h = wfPos.z();
                Vector3D normal;
                tp->getHeightAndNormal(wfPos.x(), wfPos.y(), h, normal);
                transProp->getTranslate().z() = h;
                transProp->apply(&entity);

                btQuaternion orientation = entity.m_location.m_orientation.isValid() ? Convert::toBullet(entity.m_location.m_orientation) : btQuaternion::getIdentity();
                btVector3 pos = wfPos.isValid() ? Convert::toBullet(wfPos) : btVector3(0, 0, 0);

                //"Center of mass offset" is the inverse of the center of the object in relation to origo.
                btVector3 centerOfMassOffset = -Convert::toBullet(entity.m_location.m_bBox.getCenter());

                bulletEntry->rigidBody->setWorldTransform(btTransform(orientation, pos - centerOfMassOffset));
            }
        }

        //When altering mass we need to first remove and then re-add the body, for some reason.
        m_dynamicsWorld->removeRigidBody(bulletEntry->rigidBody);

        if (modeProp->data() == "planted" || modeProp->data() == "fixed") {
            //"fixed" mode means that the entity stays in place, always
            //"planted" mode means it's planted in the ground
            //Zero mass makes the rigid body static
            bulletEntry->rigidBody->setMassProps(0, btVector3(0, 0, 0));
        } else {
            float mass = getMassForEntity(*bulletEntry->entity);
            btVector3 inertia;
            bulletEntry->collisionShape->calculateLocalInertia(mass, inertia);

            bulletEntry->rigidBody->setMassProps(mass, inertia);

        }

        short collisionMask;
        short collisionGroup;
        getCollisionFlagsForEntity(*bulletEntry->entity, collisionGroup, collisionMask);

        m_dynamicsWorld->addRigidBody(bulletEntry->rigidBody, collisionGroup, collisionMask);

        bulletEntry->rigidBody->activate();

        return;
    } else if (name == "solid") {
        short collisionMask;
        short collisionGroup;
        getCollisionFlagsForEntity(*bulletEntry->entity, collisionGroup, collisionMask);
        m_dynamicsWorld->removeRigidBody(bulletEntry->rigidBody);
        m_dynamicsWorld->addRigidBody(bulletEntry->rigidBody, collisionGroup, collisionMask);

        bulletEntry->rigidBody->activate();
    } else if (name == "mass") {

        ModeProperty* modeProp = bulletEntry->entity->requirePropertyClassFixed<ModeProperty>();

        if (modeProp->data() == "planted" || modeProp->data() == "fixed") {
            //"fixed" mode means that the entity stays in place, always
            //"planted" mode means it's planted in the ground
            //Zero mass makes the rigid body static
        } else {
            //When altering mass we need to first remove and then re-add the body, for some reason.
            m_dynamicsWorld->removeRigidBody(bulletEntry->rigidBody);

            short collisionMask;
            short collisionGroup;
            getCollisionFlagsForEntity(*bulletEntry->entity, collisionGroup, collisionMask);

            float mass = getMassForEntity(*bulletEntry->entity);
            btVector3 inertia;
            bulletEntry->collisionShape->calculateLocalInertia(mass, inertia);

            bulletEntry->rigidBody->setMassProps(mass, inertia);
            m_dynamicsWorld->addRigidBody(bulletEntry->rigidBody, collisionGroup, collisionMask);
        }

    }
}

void PhysicalDomain::getCollisionFlagsForEntity(const LocatedEntity& entity, short& collisionGroup, short& collisionMask) const
{
    collisionMask = COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN;
    collisionGroup = COLLISION_MASK_PHYSICAL;

    //Non solid objects should collide with the terrain only.
    if (!entity.m_location.isSolid()) {
        collisionMask = COLLISION_MASK_TERRAIN;
        collisionGroup = COLLISION_MASK_NON_PHYSICAL;
    }
}

void PhysicalDomain::entityPropertyApplied(const std::string& name, PropertyBase& prop)
{
    if (name == "friction") {
        Property<float>* frictionProp = static_cast<Property<float>*>(&prop);
        for (auto& entry : m_terrainSegments) {
            entry.second.rigidBody->setFriction(frictionProp->data());
            entry.second.rigidBody->activate();
        }
        return;
    }
}

void PhysicalDomain::applyTransform(LocatedEntity& entity, const WFMath::Quaternion& orientation, const WFMath::Point<3>& pos, const WFMath::Vector<3>& velocity,
        const WFMath::AxisBox<3>& bbox)
{
    auto I = m_entries.find(entity.getIntId());
    assert(I != m_entries.end());
    auto* entry = I->second;
    if (entry->rigidBody) {
        if (orientation.isValid() || pos.isValid()) {
            btTransform transform = entry->rigidBody->getWorldTransform();
            if (orientation.isValid()) {
                debug_print("PhysicalDomain::new orientation " << entity.describeEntity() << " " << orientation);
                transform.setRotation(Convert::toBullet(orientation));
            }
            if (pos.isValid()) {
                debug_print("PhysicalDomain::new pos " << entity.describeEntity() << " " << pos);
                transform.setOrigin(Convert::toBullet(pos));
            }
            entry->rigidBody->setWorldTransform(transform);
        }

        if (velocity.isValid()) {
            debug_print("PhysicalDomain::setVelocity " << entity.describeEntity() << " " << velocity << " " << velocity.mag());
            auto I = m_entries.find(entity.getIntId());
            assert(I != m_entries.end());
            auto* entry = I->second;
            if (entry->rigidBody) {
                btVector3 btVelocity = Convert::toBullet(velocity);

                if (!btVelocity.isZero()) {
                    auto I = m_propellingEntries.find(entity.getIntId());
                    if (I == m_propellingEntries.end()) {
                        m_propellingEntries.insert(std::make_pair(entity.getIntId(), std::make_pair(entry, btVelocity)));
                    } else {
                        I->second.second = btVelocity;
                    }

                } else {
                    btVector3 velocity = entry->rigidBody->getLinearVelocity();
                    velocity.setX(0);
                    velocity.setZ(0);
                    //Take gravity into account
                    if (velocity.getY() > 0) {
                        velocity.setY(0);
                    }
                    entry->rigidBody->setLinearVelocity(velocity);

                    m_propellingEntries.erase(entity.getIntId());
                }
            }

        }

        //TODO: handle scaling of bbox
    }
}

void PhysicalDomain::sendMoveSight(BulletEntry& entry)
{
    LocatedEntity& entity = *entry.entity;
    debug_print("new velocity: " << entity.m_location.velocity() << " " << entity.m_location.velocity().mag());
    Move m;
    Anonymous move_arg;
    move_arg->setId(m_entity.getId());
    entity.m_location.addToEntity(move_arg);
    m->setArgs1(move_arg);
    m->setFrom(entity.getId());
    m->setTo(entity.getId());

    Sight s;
    s->setArgs1(m);

    entity.sendWorld(s);

    std::vector<Operation> res;
    processVisibilityForMovedEntity(entity, entry.lastSentLocation, res);
    for (auto& op : res) {
        entity.sendWorld(op);
    }
    entity.onUpdated();
    entry.lastSentLocation = entity.m_location;
}

void PhysicalDomain::processMovedEntity(BulletEntry& bulletEntry)
{
    LocatedEntity& entity = *bulletEntry.entity;
    const Location& lastSentLocation = bulletEntry.lastSentLocation;
    const Location& location = entity.m_location;

//    bool orientationChange = entity.m_location.m_orientation != lastSentLocation.m_orientation;
    bool orientationChange = !location.m_orientation.isEqualTo(lastSentLocation.m_orientation, 0.1f);

    bool hadValidVelocity = lastSentLocation.m_velocity.isValid();
    bool hadZeroVelocity = lastSentLocation.m_velocity.isEqualTo(WFMath::Vector<3>::ZERO());
    bool hadZeroAngular = lastSentLocation.m_angularVelocity.isEqualTo(WFMath::Vector<3>::ZERO());
    bool xChange = !fuzzyEquals(location.m_velocity.x(), lastSentLocation.m_velocity.x(), 0.01f);
    bool yChange = !fuzzyEquals(location.m_velocity.y(), lastSentLocation.m_velocity.y(), 0.01f);
    bool zChange = !fuzzyEquals(location.m_velocity.z(), lastSentLocation.m_velocity.z(), 0.01f);

    if (false) {
        sendMoveSight(bulletEntry);
    } else {
        //Send an update if either the previous velocity was invalid, or any of the velocity components have changed enough, or if either the new or the old velocity is zero.
        if (!hadValidVelocity) {
            debug_print("No previous valid velocity " << entity.describeEntity() << " " << lastSentLocation.m_velocity);

            sendMoveSight(bulletEntry);
        } else if (xChange || yChange || zChange) {
            debug_print("Velocity changed " << entity.describeEntity() << " " << location.m_velocity);

            sendMoveSight(bulletEntry);
        } else if (entity.m_location.m_velocity.isEqualTo(WFMath::Vector<3>::ZERO()) && !hadZeroVelocity) {
            debug_print("Old or new velocity zero " << entity.describeEntity() << " " << location.m_velocity);

            sendMoveSight(bulletEntry);
        } else if (orientationChange) {
            debug_print("Orientation changed " << entity.describeEntity() << " " << location.orientation());

            sendMoveSight(bulletEntry);
        } else {
            bool angularChange = !fuzzyEquals(lastSentLocation.m_angularVelocity, location.m_angularVelocity, 0.01f);
            if (angularChange) {
                debug_print("Angular changed " << entity.describeEntity() << " " << location.m_angularVelocity);

                sendMoveSight(bulletEntry);
            } else if (entity.m_location.m_angularVelocity.isEqualTo(WFMath::Vector<3>::ZERO()) && !hadZeroAngular) {
                debug_print("Angular changed " << entity.describeEntity() << " " << location.m_angularVelocity);

                sendMoveSight(bulletEntry);
            }
        }
    }

}

double PhysicalDomain::tick(double timeNow)
{
    if (m_lastTickTime == 0) {
        m_lastTickTime = timeNow;
    }

    m_movingEntities.clear();

    float currentTickSize = timeNow - m_lastTickTime;
    m_lastTickTime = timeNow;

    m_dynamicsWorld->stepSimulation(currentTickSize, 10);

    //Check all entities that moved this tick.
    for (BulletEntry* entry : m_movingEntities) {
        //Check if the entity also moved last tick.
        if (m_lastMovingEntities.find(entry) == m_lastMovingEntities.end()) {
            //Didn't move before
            sendMoveSight(*entry);
        } else {
            processMovedEntity(*entry);
            //Erase from last moving entities, so we can find those that moved last tick, but not this.
            m_lastMovingEntities.erase(entry);
        }
    }

    for (BulletEntry* entry : m_lastMovingEntities) {
        //Stopped moving
        debug_print("Stopped moving " << entry->entity->describeEntity());
        entry->entity->m_location.m_angularVelocity.zero();
        entry->entity->m_location.m_velocity.zero();
        processMovedEntity(*entry);
    }

    //Stash those entities that moved this tick for checking next tick.
    std::swap(m_movingEntities, m_lastMovingEntities);

    return timeNow + (1.0 / m_ticksPerSecond);
}
