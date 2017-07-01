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

#endif

#include "PhysicalDomain.h"

#include "TerrainProperty.h"
#include "LocatedEntity.h"
#include "OutfitProperty.h"
#include "PropelProperty.h"
#include "GeometryProperty.h"
#include "AngularFactorProperty.h"
#include "VisibilityProperty.h"
#include "TerrainModProperty.h"
#include "PhysicalWorld.h"

#include "physics/Convert.h"

#include "common/debug.h"
#include "common/Unseen.h"
#include "common/TypeNode.h"
#include "common/Update.h"
#include "common/BaseWorld.h"

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <wfmath/atlasconv.h>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include <sigc++/bind.h>

#include <unordered_set>
#include <chrono>


static const bool debug_flag = false;

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
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Wield;

using Atlas::Objects::smart_dynamic_cast;


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
 * How much the visibility sphere should be scaled against the size of the bbox.
 */
float VISIBILITY_SCALING_FACTOR = 100;

/**
 * Mask used by visibility checks for observing entries (i.e. creatures etc.).
 */
short VISIBILITY_MASK_OBSERVER = 1;

/**
 * Mask used by visibility checks for entries that can be observed (i.e. most entities).
 */
short VISIBILITY_MASK_OBSERVABLE = 2;

/**
 * Mask used by all physical items. They should collide with other physical items, and with the terrain.
 */
short COLLISION_MASK_PHYSICAL = 1;
/**
 * Mask used by the terrain. It's static.
 */
short COLLISION_MASK_NON_PHYSICAL = 2;
/**
 * Mask used by all non-physical items. These should only collide with the terrain.
 */
short COLLISION_MASK_TERRAIN = 4;
/**
 * Mask used by static items (i.e. those with mode "fixed" and "planted").
 */
short COLLISION_MASK_STATIC = 8;

/**
 * Interval, in seconds, for doing visibility checks.
 */
float VISIBILITY_CHECK_INTERVAL_SECONDS = 2.0f;

float CCD_MOTION_FACTOR = 0.2f;

float CCD_SPHERE_FACTOR = 0.2f;

class PhysicalDomain::PhysicalMotionState : public btMotionState
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
            centerOfMassWorldTrans = m_worldTrans * m_centerOfMassOffset.inverse();
        }

        ///synchronizes world transform from physics to user
        ///Bullet only calls the update of worldtransform for active objects
        virtual void setWorldTransform(const btTransform& /* centerOfMassWorldTrans */)
        {


            LocatedEntity& entity = *m_bulletEntry.entity;
            m_domain.m_movingEntities.insert(&m_bulletEntry);
            m_domain.m_dirtyEntries.insert(&m_bulletEntry);

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
            //entity.setFlags(entity_dirty_location);

            btCollisionObject* visibilitySphere = m_bulletEntry.visibilitySphere;
            if (visibilitySphere) {
                visibilitySphere->setWorldTransform(
                    btTransform(visibilitySphere->getWorldTransform().getBasis(), m_bulletEntry.rigidBody->getWorldTransform().getOrigin() / VISIBILITY_SCALING_FACTOR));
                m_domain.m_visibilityWorld->updateSingleAabb(visibilitySphere);
            }

            btCollisionObject* viewSphere = m_bulletEntry.viewSphere;
            if (viewSphere) {
                viewSphere->setWorldTransform(btTransform(viewSphere->getWorldTransform().getBasis(), m_bulletEntry.rigidBody->getWorldTransform().getOrigin() / VISIBILITY_SCALING_FACTOR));
                m_domain.m_visibilityWorld->updateSingleAabb(viewSphere);
            }

        }
};

PhysicalDomain::PhysicalDomain(LocatedEntity& entity) :
    Domain(entity),
    //default config for now
    m_collisionConfiguration(new btDefaultCollisionConfiguration()),
    m_dispatcher(new btCollisionDispatcher(m_collisionConfiguration)),
    m_constraintSolver(new btSequentialImpulseConstraintSolver()),
    //We'll use a dynamic broadphase for the main world. It's not as fast as SAP variants, but it's faster when dynamic objects are at rest.
    m_broadphase(new btDbvtBroadphase()),
    m_dynamicsWorld(new PhysicalWorld(m_dispatcher, m_broadphase, m_constraintSolver, m_collisionConfiguration)), m_visibilityWorld(
    //We'll use a SAP broadphase for the visibility. This is more efficient than a dynamic one.
    new btCollisionWorld(new btCollisionDispatcher(new btDefaultCollisionConfiguration()),
                         new bt32BitAxisSweep3(Convert::toBullet(entity.m_location.bBox().lowCorner()),
                                               Convert::toBullet(entity.m_location.bBox().highCorner())),
                         new btDefaultCollisionConfiguration())),
    m_visibilityCheckCountdown(0),
    m_terrain(nullptr)
{

    //This is to prevent us from sliding down slopes.
    //m_dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;

    //By default all collision objects have their aabbs updated each tick; we'll disable it for performance.
    m_dynamicsWorld->setForceUpdateAllAabbs(false);

    m_visibilityWorld->setForceUpdateAllAabbs(false);

    const TerrainProperty* terrainProperty = m_entity.getPropertyClass<TerrainProperty>("terrain");
    if (terrainProperty) {
        m_terrain = &terrainProperty->getData();
    }

    createDomainBorders();

    //Update the linear velocity of all self propelling entities each tick.
    auto preTickCallback = [](btDynamicsWorld* world, btScalar timeStep) {

        std::map<int, PropelEntry>* propellingEntries = static_cast<std::map<int, PropelEntry>*>(world->getWorldUserInfo());
        for (auto& entry : *propellingEntries) {
//            const btVector3& velocity = entry.second.bulletEntry->rigidBody->getLinearVelocity();
//            entry.second.bulletEntry->rigidBody->setLinearVelocity(btVector3(entry.second.velocity.x(), velocity.y()+ (world->getGravity().y() * timeStep), entry.second.velocity.z()));

            float verticalVelocity = entry.second.bulletEntry->rigidBody->getLinearVelocity().y();

            //Apply gravity
            if (verticalVelocity != 0) {
                verticalVelocity += world->getGravity().y() * timeStep;
                entry.second.bulletEntry->rigidBody->setLinearVelocity(entry.second.velocity + btVector3(0, verticalVelocity, 0));
            } else {
                entry.second.bulletEntry->rigidBody->setLinearVelocity(entry.second.velocity);
            }

            //When entities are being propelled they will have low friction. When propelling stop the friction will be returned in setVelocity.
            entry.second.bulletEntry->rigidBody->setFriction(0.5);
            entry.second.bulletEntry->rigidBody->activate();
        }
    };

    m_dynamicsWorld->setInternalTickCallback(preTickCallback, &m_propellingEntries, true);

    mContainingEntityEntry.entity = &entity;

    m_entries.insert(std::make_pair(entity.getIntId(), &mContainingEntityEntry));

    buildTerrainPages();
}

PhysicalDomain::~PhysicalDomain()
{
    for (auto planeBody : m_borderPlanes) {
        delete planeBody->getCollisionShape();
        delete planeBody;
    }

    for (auto& entry : m_terrainSegments) {
        delete entry.second.data;
        delete entry.second.rigidBody->getCollisionShape();
        delete entry.second.rigidBody;
    }

    //Remove our own entry first, since we own the memory
    m_entries.erase(m_entity.getIntId());

    for (auto& entry : m_entries) {
        if (entry.second->rigidBody) {
            m_dynamicsWorld->removeRigidBody(entry.second->rigidBody);
            delete entry.second->motionState;
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
    // delete m_visibilityWorld->getBroadphase();
    delete m_visibilityWorld;
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
    if (terrainEntry.rigidBody) {
        m_dynamicsWorld->removeRigidBody(terrainEntry.rigidBody);
        delete terrainEntry.rigidBody->getCollisionShape();
        delete terrainEntry.rigidBody;
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

    float res = (float) segment.getResolution();

    float xPos = segment.getXRef() + (res / 2);
    float yPos = segment.getYRef() + (res / 2);
    float zPos = min + ((max - min) * 0.5f);

    WFMath::Point<3> pos(xPos, yPos, zPos);
    btVector3 btPos = Convert::toBullet(pos);


    btRigidBody::btRigidBodyConstructionInfo segmentCI(.0f, nullptr, terrainShape);
    segmentCI.m_friction = friction;
    btRigidBody* segmentBody = new btRigidBody(segmentCI);
    segmentBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), btPos));

    m_dynamicsWorld->addRigidBody(segmentBody, COLLISION_MASK_TERRAIN, COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL);

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
                btStaticPlaneShape* plane = new btStaticPlaneShape(normal, .0f);
                btRigidBody* planeBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, nullptr, plane));
                planeBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), translate));

                m_dynamicsWorld->addRigidBody(planeBody, COLLISION_MASK_TERRAIN, COLLISION_MASK_NON_PHYSICAL | COLLISION_MASK_PHYSICAL);
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
    //Is it observing the domain entity?
    if (&observedEntity == &m_entity) {
        return true;
    }

    //Is it observing itself?
    if (&observingEntity == &observedEntity) {
        return true;
    }

    //Is it the domain entity?
    if (&observingEntity == &m_entity) {
        return true;
    }

    auto observingI = m_entries.find(observingEntity.getIntId());
    if (observingI == m_entries.end()) {
        return false;
    }
    auto observedI = m_entries.find(observedEntity.getIntId());
    if (observedI == m_entries.end()) {
        return false;
    }

    BulletEntry* observedEntry = observedI->second;
    BulletEntry* observingEntry = observingI->second;
    return observedEntry->observingThis.find(observingEntry) != observedEntry->observingThis.end();
}

void PhysicalDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    auto observingI = m_entries.find(observingEntity.getIntId());
    if (observingI != m_entries.end()) {
        const BulletEntry* bulletEntry = observingI->second;
        for (const auto& observedEntry : bulletEntry->observedByThis) {
            entityList.push_back(observedEntry->entity);
        }
    }
}

std::list<LocatedEntity*> PhysicalDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::list<LocatedEntity*> entityList;

    auto observedI = m_entries.find(observedEntity.getIntId());
    if (observedI != m_entries.end()) {
        const BulletEntry* bulletEntry = observedI->second;
        for (const auto& observingEntry : bulletEntry->observingThis) {
            entityList.push_back(observingEntry->entity);
        }
    }

    return std::move(entityList);
}

class PhysicalDomain::VisibilityCallback : public btCollisionWorld::ContactResultCallback
{
    public:

        std::set<BulletEntry*> m_entries;

        virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap,
                                         int partId1, int index1)
        {
            BulletEntry* bulletEntry = static_cast<BulletEntry*>(colObj1Wrap->m_collisionObject->getUserPointer());
            if (bulletEntry) {
                m_entries.insert(bulletEntry);
            }
            return btScalar(1.0);
        }

};

void PhysicalDomain::updateObserverEntry(BulletEntry* bulletEntry, OpVector& res)
{

    if (bulletEntry->viewSphere) {
        //This entry is an observer; check what it can see after it has moved
        VisibilityCallback callback;

        //callback.m_filterOutEntry = bulletEntry;
        debug_print("Updating what can be observed by entity " << bulletEntry->entity->describeEntity());
        callback.m_entries.clear();

        debug_print(" " << bulletEntry->entity->describeEntity() << " viewSphere: " << bulletEntry->viewSphere->getWorldTransform().getOrigin());

        if (bulletEntry->entity->m_location.m_pos.isValid()) {
            callback.m_collisionFilterGroup = VISIBILITY_MASK_OBSERVABLE;
            callback.m_collisionFilterMask = VISIBILITY_MASK_OBSERVER;
            m_visibilityWorld->contactTest(bulletEntry->viewSphere, callback);
        }

        debug_print(" observed by " << bulletEntry->entity->describeEntity() << ": " << callback.m_entries.size());

        auto& observed = bulletEntry->observedByThis;

        //See which entities became visible, and which sight was lost of.
        for (BulletEntry* viewedEntry : callback.m_entries) {
            if (viewedEntry == bulletEntry) {
                continue;
            }
            auto I = observed.find(viewedEntry);
            if (I != observed.end()) {
                //It was already seen; do nothing special
                observed.erase(I);
            } else {
                //Send Appear
                // debug_print(" appear: " << viewedEntry->entity->describeEntity() << " for " << bulletEntry->entity->describeEntity());
                Appearance appear;
                Anonymous that_ent;
                that_ent->setId(viewedEntry->entity->getId());
                that_ent->setStamp(viewedEntry->entity->getSeq());
                appear->setArgs1(that_ent);
                appear->setTo(bulletEntry->entity->getId());
                res.push_back(appear);

                viewedEntry->observingThis.insert(bulletEntry);
            }
        }

        for (BulletEntry* disappearedEntry : observed) {
            if (disappearedEntry == bulletEntry) {
                continue;
            }
            //Send disappearence
            //debug_print(" disappear: " << disappearedEntry->entity->describeEntity() << " for " << bulletEntry->entity->describeEntity());
            Disappearance disappear;
            Anonymous that_ent;
            that_ent->setId(disappearedEntry->entity->getId());
            that_ent->setStamp(disappearedEntry->entity->getSeq());
            disappear->setArgs1(that_ent);
            disappear->setTo(bulletEntry->entity->getId());
            res.push_back(disappear);

            disappearedEntry->observingThis.erase(bulletEntry);
        }

        bulletEntry->observedByThis = std::move(callback.m_entries);
        //Make sure ourselves is in the list
        bulletEntry->observedByThis.insert(bulletEntry);
    }
}


void PhysicalDomain::updateObservedEntry(BulletEntry* bulletEntry, OpVector& res, bool generateOps)
{
    if (bulletEntry->visibilitySphere) {
        //This entry is something which can be observed; check what can see it after it has moved

        VisibilityCallback callback;

        debug_print("Updating what is observing entity " << bulletEntry->entity->describeEntity());
        debug_print(" " << bulletEntry->entity->describeEntity() << " visibilitySphere: " << bulletEntry->visibilitySphere->getWorldTransform().getOrigin());
        callback.m_entries.clear();

        if (bulletEntry->entity->m_location.m_pos.isValid()) {
            callback.m_collisionFilterGroup = VISIBILITY_MASK_OBSERVER;
            callback.m_collisionFilterMask = VISIBILITY_MASK_OBSERVABLE;
            m_visibilityWorld->contactTest(bulletEntry->visibilitySphere, callback);
        }

        debug_print(" observing " << bulletEntry->entity->describeEntity() << ": " << callback.m_entries.size());

        auto& observing = bulletEntry->observingThis;
        //See which entities got sight of this, and for which sight was lost.
        for (BulletEntry* viewingEntry : callback.m_entries) {
            auto I = observing.find(viewingEntry);
            if (I != observing.end()) {
                //It was already seen; do nothing special
                observing.erase(I);
            } else {
                if (generateOps) {
                    //Send appear
                    // debug_print(" appear: " << bulletEntry->entity->describeEntity() << " for " << viewingEntry->entity->describeEntity());
                    Appearance appear;
                    Anonymous that_ent;
                    that_ent->setId(bulletEntry->entity->getId());
                    that_ent->setStamp(bulletEntry->entity->getSeq());
                    appear->setArgs1(that_ent);
                    appear->setTo(viewingEntry->entity->getId());
                    res.push_back(appear);
                }

                viewingEntry->observedByThis.insert(bulletEntry);
            }
        }

        for (BulletEntry* noLongerObservingEntry : observing) {

            if (generateOps) {
                //Send disappearence
                // debug_print(" disappear: " << bulletEntry->entity->describeEntity() << " for " << noLongerObservingEntry->entity->describeEntity());
                Disappearance disappear;
                Anonymous that_ent;
                that_ent->setId(bulletEntry->entity->getId());
                that_ent->setStamp(bulletEntry->entity->getSeq());
                disappear->setArgs1(that_ent);
                disappear->setTo(noLongerObservingEntry->entity->getId());
                res.push_back(disappear);
            }

            noLongerObservingEntry->observedByThis.erase(bulletEntry);
        }

        bulletEntry->observingThis = std::move(callback.m_entries);

    }
}

void PhysicalDomain::updateVisibilityOfDirtyEntities(OpVector& res)
{
    for (auto& bulletEntry : m_dirtyEntries) {
        updateObservedEntry(bulletEntry, res);
        updateObserverEntry(bulletEntry, res);
        bulletEntry->entity->onUpdated();
    }
    m_dirtyEntries.clear();
}

float PhysicalDomain::getMassForEntity(const LocatedEntity& entity) const
{
    float mass = 0;

    if (entity.getType()->isTypeOf("creator")) {
        mass = 1.0f;
    }

    auto massProp = entity.getPropertyType<double>("mass");
    if (massProp) {
        mass = (float) massProp->data();
    }
    return mass;
}

void PhysicalDomain::addEntity(LocatedEntity& entity)
{
    assert(m_entries.find(entity.getIntId()) == m_entries.end());

    float mass = getMassForEntity(entity);

    WFMath::AxisBox<3> bbox = entity.m_location.bBox();
    btVector3 angularFactor(1, 1, 1);

    BulletEntry* entry = new BulletEntry();
    m_entries.insert(std::make_pair(entity.getIntId(), entry));
    entry->entity = &entity;

    //Handle the special case of the entity being a "creator".
    if (entity.getType()->isTypeOf("creator")) {
        if (!bbox.isValid()) {
            bbox = WFMath::AxisBox<3>(WFMath::Point<3>(-0.25f, -0.25f, .0f), WFMath::Point<3>(0.25f, 0.25f, 1.5f));
        }
        angularFactor = btVector3(0, 0, 0);
    }

    const AngularFactorProperty* angularFactorProp = entity.getPropertyClassFixed<AngularFactorProperty>();
    if (angularFactorProp && angularFactorProp->data().isValid()) {
        angularFactor = Convert::toBullet(angularFactorProp->data());
    }

    ModeProperty::Mode mode = ModeProperty::Mode::Free;
    auto modeProp = entity.getPropertyClassFixed<ModeProperty>();
    if (modeProp) {
        mode = modeProp->getMode();
    }

    if (mode == ModeProperty::Mode::Planted || mode == ModeProperty::Mode::Fixed) {
        //"fixed" mode means that the entity stays in place, always
        //"planted" mode means it's planted in the ground
        //Zero mass makes the rigid body static
        mass = .0f;
    }

    calculatePositionForEntity(mode, entity, entity.m_location.m_pos);


    btQuaternion orientation = entity.m_location.m_orientation.isValid() ? Convert::toBullet(entity.m_location.m_orientation) : btQuaternion(0, 0, 0, 1);
    btVector3 pos = entity.m_location.m_pos.isValid() ? Convert::toBullet(entity.m_location.m_pos) : btVector3(0, 0, 0);

    if (bbox.isValid()) {
        //"Center of mass offset" is the inverse of the center of the object in relation to origo.

        auto size = bbox.highCorner() - bbox.lowCorner();
        const GeometryProperty* geometryProp = entity.getPropertyClassFixed<GeometryProperty>();
        if (geometryProp) {
            std::pair<btCollisionShape*, std::shared_ptr<btCollisionShape>> instance = geometryProp->createShape(bbox, entry->centerOfMassOffset);
            entry->collisionShape = instance.first;
            entry->backingShape = instance.second;
        } else {
            auto btSize = Convert::toBullet(size * 0.5).absolute();
            entry->centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
            entry->collisionShape = new btBoxShape(btSize);
        }

        short collisionMask;
        short collisionGroup;
        getCollisionFlagsForEntity(entity, collisionGroup, collisionMask);

        btVector3 inertia;
        if (mass == 0) {
            inertia = btVector3(0, 0, 0);
        } else {
            entry->collisionShape->calculateLocalInertia(mass, inertia);
        }

        debug_print(
            "PhysicsDomain adding entity " << entity.describeEntity() << " with mass " << mass << " and inertia (" << inertia.x() << "," << inertia.y() << "," << inertia.z() << ")");

        btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, nullptr, entry->collisionShape, inertia);

        const Property<float>* frictionProp = entity.getPropertyType<float>("friction");
        if (frictionProp) {
            rigidBodyCI.m_friction = frictionProp->data();
        }

        entry->rigidBody = new btRigidBody(rigidBodyCI);
        entry->motionState = new PhysicalMotionState(*entry, *this, btTransform(orientation, pos), btTransform(btQuaternion::getIdentity(), entry->centerOfMassOffset));
        entry->rigidBody->setMotionState(entry->motionState);
        entry->rigidBody->setAngularFactor(angularFactor);
        entry->rigidBody->setUserPointer(entry);

        //To prevent tunneling we'll turn on CCD with suitable values.
        float minSize = std::min(size.x(), std::min(size.y(), size.z()));
        float maxSize = std::max(size.x(), std::max(size.y(), size.z()));
        entry->rigidBody->setCcdMotionThreshold(minSize * CCD_MOTION_FACTOR);
        entry->rigidBody->setCcdSweptSphereRadius(minSize * CCD_SPHERE_FACTOR);

        if (mass == 0) {
            entry->rigidBody->setCollisionFlags(entry->rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        }

        //Only add to world if position is valid. Otherwise this will be done when a new valid position is applied in applyNewPositionForEntity
        if (entity.m_location.m_pos.isValid()) {
            m_dynamicsWorld->addRigidBody(entry->rigidBody, collisionGroup, collisionMask);
        }

        //Call to "activate" will be ignored for bodies marked with CF_STATIC_OBJECT
        entry->rigidBody->activate();

        const PropelProperty* propelProp = entity.getPropertyClassFixed<PropelProperty>();
        if (propelProp && propelProp->data().isValid() && propelProp->data() != WFMath::Vector<3>::ZERO()) {
            applyVelocity(*entry, propelProp->data());
        }


        entry->propertyUpdatedConnection = entity.propertyApplied.connect(sigc::bind(sigc::mem_fun(this, &PhysicalDomain::childEntityPropertyApplied), entry));

    }

    updateTerrainMod(entity, true);

    {

        btSphereShape* visSphere = new btSphereShape(0);
        const VisibilityProperty* visProp = entity.getPropertyClass<VisibilityProperty>("visibility");
        if (visProp) {
            visSphere->setUnscaledRadius(visProp->data() / VISIBILITY_SCALING_FACTOR);
        } else if (entity.m_location.bBox().isValid() && entity.m_location.radius() > 0) {
            float radius = entity.m_location.radius();
            visSphere->setUnscaledRadius(radius);
        } else {
            visSphere->setUnscaledRadius(0.25f);
        }

        btCollisionObject* visObject = new btCollisionObject();
        visObject->setCollisionShape(visSphere);
        visObject->setWorldTransform(btTransform(btQuaternion::getIdentity(), pos / VISIBILITY_SCALING_FACTOR));
        visObject->setUserPointer(entry);
        entry->visibilitySphere = visObject;
        if (entity.m_location.m_pos.isValid()) {
            m_visibilityWorld->addCollisionObject(visObject, VISIBILITY_MASK_OBSERVER, VISIBILITY_MASK_OBSERVABLE);
        }
    }
    if (entity.isPerceptive()) {
        btSphereShape* viewSphere = new btSphereShape(0.5f / VISIBILITY_SCALING_FACTOR);
        btCollisionObject* visObject = new btCollisionObject();
        visObject->setCollisionShape(viewSphere);
        visObject->setWorldTransform(btTransform(btQuaternion::getIdentity(), pos / VISIBILITY_SCALING_FACTOR));
        visObject->setUserPointer(entry);
        entry->viewSphere = visObject;
        mContainingEntityEntry.observingThis.insert(entry);
        if (entity.m_location.m_pos.isValid()) {
            m_visibilityWorld->addCollisionObject(visObject, VISIBILITY_MASK_OBSERVABLE, VISIBILITY_MASK_OBSERVER);
        }
    }

    if (m_entity.isPerceptive()) {
        entry->observingThis.insert(&mContainingEntityEntry);
        mContainingEntityEntry.observedByThis.insert(entry);
    }

    OpVector res;
    updateObserverEntry(entry, res);
    updateObservedEntry(entry, res, false); //Don't send any ops, since that will be handled by the calling code when changing locations.
    for (auto& op : res) {
        m_entity.sendWorld(op);
    }
}

void PhysicalDomain::toggleChildPerception(LocatedEntity& entity)
{
    auto I = m_entries.find(entity.getIntId());
    assert(I != m_entries.end());
    BulletEntry* entry = I->second;
    if (entity.isPerceptive()) {
        if (!entry->viewSphere) {
            mContainingEntityEntry.observingThis.insert(entry);
            btSphereShape* viewSphere = new btSphereShape(0.5f / VISIBILITY_SCALING_FACTOR);
            btCollisionObject* visObject = new btCollisionObject();
            visObject->setCollisionShape(viewSphere);
            visObject->setUserPointer(entry);
            entry->viewSphere = visObject;
            if (entity.m_location.m_pos.isValid()) {
                visObject->setWorldTransform(btTransform(btQuaternion::getIdentity(), Convert::toBullet(entity.m_location.m_pos) / VISIBILITY_SCALING_FACTOR));
                m_visibilityWorld->addCollisionObject(visObject, VISIBILITY_MASK_OBSERVABLE, VISIBILITY_MASK_OBSERVER);
            }
            OpVector res;
            updateObserverEntry(entry, res);
            for (auto& op : res) {
                m_entity.sendWorld(op);
            }
        }
    } else {
        if (entry->viewSphere) {
            m_visibilityWorld->removeCollisionObject(entry->viewSphere);
            delete entry->viewSphere->getCollisionShape();
            delete entry->viewSphere;
            entry->viewSphere = nullptr;
            mContainingEntityEntry.observingThis.erase(entry);
        }
    }
}

void PhysicalDomain::removeEntity(LocatedEntity& entity)
{
    debug_print("PhysicalDomain::removeEntity " << entity.describeEntity());
    auto I = m_entries.find(entity.getIntId());
    assert(I != m_entries.end());
    BulletEntry* entry = I->second;

    auto modI = m_terrainMods.find(entity.getIntId());
    if (modI != m_terrainMods.end()) {
        m_terrain->updateMod(entity.getIntId(), nullptr);
        m_terrainMods.erase(modI);
    }

    m_lastMovingEntities.erase(entry);
    if (entry->rigidBody) {
        m_dynamicsWorld->removeRigidBody(entry->rigidBody);
        delete entry->motionState;
        delete entry->rigidBody;
    }
    if (entry->collisionShape) {
        delete entry->collisionShape;
    }
    entry->propertyUpdatedConnection.disconnect();
    if (entry->viewSphere) {
        m_visibilityWorld->removeCollisionObject(entry->viewSphere);
        delete entry->viewSphere;
    }
    if (entry->visibilitySphere) {
        m_visibilityWorld->removeCollisionObject(entry->visibilitySphere);
        delete entry->visibilitySphere;
    }
    for (BulletEntry* observer : entry->observingThis) {
        observer->observedByThis.erase(entry);
    }
    for (BulletEntry* observedEntry : entry->observedByThis) {
        observedEntry->observingThis.erase(entry);
    }

    m_dirtyEntries.erase(entry);
    mContainingEntityEntry.observingThis.erase(entry);

    //The entity owning the domain should normally not be perceptive, so we'll check first to optimize a bit.
    if (m_entity.isPerceptive()) {
        mContainingEntityEntry.observedByThis.insert(entry);
    }

    delete I->second;
    m_entries.erase(I);

    m_propellingEntries.erase(entity.getIntId());
}

void PhysicalDomain::childEntityPropertyApplied(const std::string& name, PropertyBase& prop, BulletEntry* bulletEntry)
{

    auto adjustToTerrainFn = [&]() {
        LocatedEntity& entity = *bulletEntry->entity;

        if (m_terrain) {
            WFMath::Point<3>& wfPos = entity.m_location.m_pos;

            float h = wfPos.z();
            getTerrainHeight(wfPos.x(), wfPos.y(), h);
            wfPos.z() = h;

            btQuaternion orientation = entity.m_location.m_orientation.isValid() ? Convert::toBullet(entity.m_location.m_orientation) : btQuaternion::getIdentity();
            btVector3 pos = wfPos.isValid() ? Convert::toBullet(wfPos) : btVector3(0, 0, 0);

            //"Center of mass offset" is the inverse of the center of the object in relation to origo.
            btVector3 centerOfMassOffset = -Convert::toBullet(entity.m_location.m_bBox.getCenter());

            bulletEntry->rigidBody->setWorldTransform(btTransform(orientation, pos - centerOfMassOffset));
        }
    };

    if (name == "friction") {
        if (bulletEntry->rigidBody) {
            Property<float>* frictionProp = static_cast<Property<float>*>(&prop);
            bulletEntry->rigidBody->setFriction(frictionProp->data());
            if (getMassForEntity(*bulletEntry->entity) != 0) {
                bulletEntry->rigidBody->activate();
            }
        }
        return;
    } else if (name == "mode") {

        if (bulletEntry->rigidBody) {
            ModeProperty* modeProp = static_cast<ModeProperty*>(&prop);

            applyNewPositionForEntity(bulletEntry, bulletEntry->entity->m_location.m_pos);

            //When altering mass we need to first remove and then re-add the body, for some reason.
            m_dynamicsWorld->removeRigidBody(bulletEntry->rigidBody);

            float mass = 0;
            if (modeProp->getMode() == ModeProperty::Mode::Planted || modeProp->getMode() == ModeProperty::Mode::Fixed) {
                //"fixed" mode means that the entity stays in place, always
                //"planted" mode means it's planted in the ground
                //Zero mass makes the rigid body static
                bulletEntry->rigidBody->setMassProps(0, btVector3(0, 0, 0));
                updateTerrainMod(*bulletEntry->entity);
            } else {
                mass = getMassForEntity(*bulletEntry->entity);
                btVector3 inertia;
                bulletEntry->collisionShape->calculateLocalInertia(mass, inertia);

                bulletEntry->rigidBody->setMassProps(mass, inertia);

            }

            short collisionMask;
            short collisionGroup;
            getCollisionFlagsForEntity(*bulletEntry->entity, collisionGroup, collisionMask);

            m_dynamicsWorld->addRigidBody(bulletEntry->rigidBody, collisionGroup, collisionMask);

            bulletEntry->rigidBody->activate();

            m_movingEntities.insert(bulletEntry);
            //sendMoveSight(*bulletEntry);
        }
        return;
    } else if (name == "solid") {
        short collisionMask;
        short collisionGroup;
        getCollisionFlagsForEntity(*bulletEntry->entity, collisionGroup, collisionMask);
        if (bulletEntry->rigidBody) {
            m_dynamicsWorld->removeRigidBody(bulletEntry->rigidBody);
            m_dynamicsWorld->addRigidBody(bulletEntry->rigidBody, collisionGroup, collisionMask);

            bulletEntry->rigidBody->activate();
        }
    } else if (name == "mass") {

        ModeProperty* modeProp = bulletEntry->entity->requirePropertyClassFixed<ModeProperty>();

        if (modeProp->getMode() == ModeProperty::Mode::Planted || modeProp->getMode() == ModeProperty::Mode::Fixed) {
            //"fixed" mode means that the entity stays in place, always
            //"planted" mode means it's planted in the ground
            //Zero mass makes the rigid body static
        } else {
            if (bulletEntry->rigidBody) {
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

    } else if (name == "bbox") {
        const auto& bbox = bulletEntry->entity->m_location.bBox();
        if (bbox.isValid()) {
            if (bulletEntry->rigidBody) {
                btCollisionShape* collisionShape = bulletEntry->collisionShape;
                btVector3 aabbMin, aabbMax;
                collisionShape->getAabb(btTransform::getIdentity(), aabbMin, aabbMax);
                btVector3 originalSize = (aabbMax - aabbMin) / collisionShape->getLocalScaling();
                btVector3 newSize = Convert::toBullet(bbox.highCorner() - bbox.lowCorner());

                collisionShape->setLocalScaling(newSize / originalSize);

                //"Center of mass offset" is the inverse of the center of the object in relation to origo.
                btVector3 centerOfMassOffset = -Convert::toBullet(bbox.getCenter());
                bulletEntry->motionState->m_centerOfMassOffset = btTransform(btQuaternion::getIdentity(), centerOfMassOffset);

                ModeProperty* modeProp = bulletEntry->entity->requirePropertyClassFixed<ModeProperty>();

                if (modeProp->getMode() != ModeProperty::Mode::Fixed) {
                    adjustToTerrainFn();
                }

                if (bulletEntry->rigidBody->getInvMass() != 0) {
                    bulletEntry->rigidBody->activate();
                }
            }
        }
    } else if (name == "planted-offset" || name == "planted-scaled-offset") {
        applyNewPositionForEntity(bulletEntry, bulletEntry->entity->m_location.m_pos);
        bulletEntry->entity->m_location.update(BaseWorld::instance().getTime());
        bulletEntry->entity->resetFlags(entity_clean);
        //sendMoveSight(*bulletEntry);
    } else if (name == TerrainModProperty::property_name) {
        updateTerrainMod(*bulletEntry->entity, true);
    }
}

void PhysicalDomain::updateTerrainMod(const LocatedEntity& entity, bool forceUpdate)
{
    auto modeProp = entity.getPropertyClassFixed<ModeProperty>();
    if (modeProp) {
        if (modeProp->getMode() == ModeProperty::Mode::Planted) {
            auto terrainModProperty = entity.getPropertyClassFixed<TerrainModProperty>();
            if (terrainModProperty && m_terrain) {
                //We need to get the vertical position in the terrain, without any mods.
                Mercator::Segment* segment = m_terrain->getSegmentAtPos(entity.m_location.m_pos.x(), entity.m_location.m_pos.y());
                WFMath::Point<3> modPos = entity.m_location.m_pos;
                if (segment) {
                    std::vector<WFMath::AxisBox<2>> terrainAreas;

                    //If there's no mods we can just use position right away
                    if (segment->getMods().empty()) {
                        if (!segment->isValid()) {
                            segment->populate();
                        }
                        segment->getHeight(modPos.x() - (segment->getXRef()), modPos.y() - (segment->getYRef()), modPos.z());
                    } else {
                        Mercator::HeightMap heightMap((unsigned int) segment->getResolution());
                        heightMap.allocate();
                        segment->populateHeightMap(heightMap);

                        heightMap.getHeight(modPos.x() - (segment->getXRef()), modPos.y() - (segment->getYRef()), modPos.z());
                    }

                    auto I = m_terrainMods.find(entity.getIntId());
                    Mercator::TerrainMod* oldMod = nullptr;
                    if (I != m_terrainMods.end()) {
                        oldMod = std::get<0>(I->second);
                        const WFMath::Point<3>& oldPos = std::get<1>(I->second);
                        const WFMath::Quaternion& oldOrient = std::get<2>(I->second);

                        if (!oldOrient.isEqualTo(entity.m_location.m_orientation) || !oldPos.isEqualTo(modPos)) {
                            //Need to update terrain mod
                            forceUpdate = true;
                            const WFMath::AxisBox<2>& oldArea = std::get<3>(I->second);
                            if (oldArea.isValid()) {
                                terrainAreas.push_back(oldArea);
                            }
                        }
                    } else {
                        forceUpdate = true;
                    }

                    if (forceUpdate) {
                        Mercator::TerrainMod* modifier = terrainModProperty->parseModData(modPos, entity.m_location.m_orientation);

                        m_terrain->updateMod(entity.getIntId(), modifier);
                        delete oldMod;
                        if (modifier) {
                            m_terrainMods[entity.getIntId()] = std::make_tuple(modifier, modPos, entity.m_location.m_orientation, modifier->bbox());
                            terrainAreas.push_back(modifier->bbox());
                        } else {
                            m_terrainMods.erase(entity.getIntId());
                        }

                        refreshTerrain(terrainAreas);
                    }
                }
            }
        }
    }
}

void PhysicalDomain::getCollisionFlagsForEntity(const LocatedEntity& entity, short& collisionGroup, short& collisionMask) const
{
    //The "group" defines the features of this object, which other bodies can mask out.
    //The "mask" defines the other kind of object this body will react with.

    auto modeProp = entity.getPropertyClassFixed<ModeProperty>();
    if (modeProp && (modeProp->getMode() == ModeProperty::Mode::Fixed || modeProp->getMode() == ModeProperty::Mode::Planted)) {
        if (entity.m_location.isSolid()) {
            collisionGroup = COLLISION_MASK_STATIC;
            //Planted and fixed entities shouldn't collide with anything themselves.
            //Other physical entities should however collide with them.
            collisionMask = COLLISION_MASK_PHYSICAL;
        } else {
            //The object is both fixed/planted and not solid. It shouldn't collide with anything at all.
            collisionGroup = 0;
            collisionMask = 0;
        }
    } else {
        if (entity.m_location.isSolid()) {
            //This is a physical object
            collisionGroup = COLLISION_MASK_PHYSICAL;
            //In this case other physical moving object, the terrain and all static objects.
            collisionMask = COLLISION_MASK_PHYSICAL | COLLISION_MASK_TERRAIN | COLLISION_MASK_STATIC;
        } else {
            //Non solid objects should collide with the terrain only.
            //Mark the object as non-physical
            collisionGroup = COLLISION_MASK_NON_PHYSICAL;
            //And only collide with the terrain.
            collisionMask = COLLISION_MASK_TERRAIN;
        }
    }
}

void PhysicalDomain::entityPropertyApplied(const std::string& name, PropertyBase& prop)
{
    if (name == "friction") {
        Property<float>* frictionProp = static_cast<Property<float>*>(&prop);
        for (auto& entry : m_terrainSegments) {
            entry.second.rigidBody->setFriction(frictionProp->data());
        }
        return;
    } else if (name == "terrain") {
        const TerrainProperty* terrainProperty = m_entity.getPropertyClass<TerrainProperty>("terrain");
        if (terrainProperty) {
            m_terrain = &terrainProperty->getData();
        }
    }
}

void PhysicalDomain::calculatePositionForEntity(ModeProperty::Mode mode, LocatedEntity& entity, WFMath::Point<3>& pos)
{
    if (mode == ModeProperty::Mode::Planted || mode == ModeProperty::Mode::Free) {
        float h = pos.z();
        getTerrainHeight(pos.x(), pos.y(), h);

        if (mode == ModeProperty::Mode::Planted) {
            pos.z() = h;

            auto plantedOffsetProp = entity.getPropertyType<double>("planted-offset");
            if (plantedOffsetProp) {
                pos.z() += plantedOffsetProp->data();
            }
            auto plantedScaledOffsetProp = entity.getPropertyType<double>("planted-scaled-offset");
            if (plantedScaledOffsetProp && entity.m_location.bBox().isValid()) {
                auto size = entity.m_location.bBox().highCorner() - entity.m_location.bBox().lowCorner();

                pos.z() += (plantedScaledOffsetProp->data() * size.z());
            }
        } else if (mode == ModeProperty::Mode::Free) {
            //For free entities we only want to clamp to terrain if the entity is below it
            pos.z() = std::max(pos.z(), h);
        }
    } else if (mode != ModeProperty::Mode::Fixed) {
        float h = pos.z();
        getTerrainHeight(pos.x(), pos.y(), h);
        pos.z() = h;
    }
}

void PhysicalDomain::applyNewPositionForEntity(BulletEntry* entry, const WFMath::Point<3>& pos)
{
    btCollisionObject* collObject = entry->rigidBody;
    if (collObject) {
        btTransform& transform = collObject->getWorldTransform();
        LocatedEntity& entity = *entry->entity;

        ModeProperty::Mode mode = ModeProperty::Mode::Free;
        auto modeProp = entity.getPropertyClassFixed<ModeProperty>();
        if (modeProp) {
            mode = modeProp->getMode();
        }

        WFMath::Point<3> newPos = pos;

        calculatePositionForEntity(mode, entity, newPos);

        entity.m_location.m_pos = newPos;

        debug_print("PhysicalDomain::new pos " << entity.describeEntity() << " " << pos);
        transform.setOrigin(Convert::toBullet(newPos) - entry->centerOfMassOffset);
        collObject->setWorldTransform(transform);
        //Since we've deactivated automatic updating of all aabbs each tick we need to do it ourselves when updating the position of a non-active object.
        if (!collObject->isActive()) {
            m_dynamicsWorld->updateSingleAabb(collObject);
        }
        if (entry->viewSphere) {
            entry->viewSphere->setWorldTransform(btTransform(transform.getBasis(), transform.getOrigin() / VISIBILITY_SCALING_FACTOR));
            m_visibilityWorld->updateSingleAabb(entry->viewSphere);
        }
        if (entry->visibilitySphere) {
            entry->visibilitySphere->setWorldTransform(btTransform(transform.getBasis(), transform.getOrigin() / VISIBILITY_SCALING_FACTOR));
            m_visibilityWorld->updateSingleAabb(entry->visibilitySphere);
        }

        m_movingEntities.insert(entry);
        m_dirtyEntries.insert(entry);
    }
}

void PhysicalDomain::applyVelocity(BulletEntry& entry, const WFMath::Vector<3>& velocity)
{
    /**
                 * A callback which checks if the instance is "grounded", i.e. that there's a contact point which is below its center.
                 */
    struct IsGroundedCallback : public btCollisionWorld::ContactResultCallback
    {
        const btRigidBody& m_body;
        bool& m_isGrounded;

        IsGroundedCallback(const btRigidBody& body, bool& isGrounded)
            : btCollisionWorld::ContactResultCallback(), m_body(body), m_isGrounded(isGrounded)
        {
            m_collisionFilterGroup = body.getBroadphaseHandle()->m_collisionFilterGroup;
            m_collisionFilterMask = body.getBroadphaseHandle()->m_collisionFilterMask;
        }


        virtual btScalar addSingleResult(btManifoldPoint& cp,
                                         const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                                         const btCollisionObjectWrapper* colObj1, int partId1, int index1)
        {
            //Local collision point, in the body's space
            btVector3 point;
            if (colObj0->m_collisionObject == &m_body) {
                point = cp.m_localPointA;
            } else {
                point = cp.m_localPointB;
            }

            if (point.y() <= 0) {
                m_isGrounded = true;
            }

            //Returned result is ignored.
            return 0;
        }
    };

    if (velocity.isValid()) {
        LocatedEntity* entity = entry.entity;
        if (entry.rigidBody) {

            debug_print("PhysicalDomain::setVelocity " << entity->describeEntity() << " " << velocity << " " << velocity.mag());

            btVector3 btVelocity = Convert::toBullet(velocity);

            if (!btVelocity.isZero()) {

                //Check if we're trying to jump
                if (btVelocity.m_floats[1] > 0) {
                    bool isGrounded = false;
                    IsGroundedCallback groundedCallback(*entry.rigidBody, isGrounded);
                    m_dynamicsWorld->contactTest(entry.rigidBody, groundedCallback);
                    if (isGrounded) {
                        //If the entity is grounded, allow it to jump by setting the vertical velocity.
                        btVector3 newVelocity = entry.rigidBody->getLinearVelocity();
                        newVelocity.m_floats[1] = btVelocity.m_floats[1] * 2.0f;
                        entry.rigidBody->setLinearVelocity(newVelocity);
                    }
                }
                btVelocity.m_floats[1] = 0; //Don't allow vertical velocity to be set for the continuous velocity.


                auto K = m_propellingEntries.find(entity->getIntId());
                if (K == m_propellingEntries.end()) {
                    const Property<double>* stepFactorProp = entity->getPropertyType<double>("step_factor");
                    if (stepFactorProp && entity->m_location.bBox().isValid()) {
                        float height = entity->m_location.bBox().upperBound(2) - entity->m_location.bBox().lowerBound(2);
                        m_propellingEntries.insert(std::make_pair(entity->getIntId(), PropelEntry{&entry, btVelocity, height * (float) stepFactorProp->data()}));
                    } else {
                        m_propellingEntries.insert(std::make_pair(entity->getIntId(), PropelEntry{&entry, btVelocity, 0}));
                    }
                } else {
                    K->second.velocity = btVelocity;
                }
            } else {
                btVector3 bodyVelocity = entry.rigidBody->getLinearVelocity();
                bodyVelocity.setX(0);
                bodyVelocity.setZ(0);

                //When a character stops moving, we'll also stop any vertical movement only if the character is touching the ground.
                bool isGrounded = false;
                IsGroundedCallback groundedCallback(*entry.rigidBody, isGrounded);
                m_dynamicsWorld->contactTest(entry.rigidBody, groundedCallback);
                if (isGrounded) {
                    bodyVelocity.setY(0);
                }

                entry.rigidBody->setLinearVelocity(bodyVelocity);
                float friction = 1.0f; //Default to 1 if no "friction" prop is present.
                const Property<float>* frictionProp = entity->getPropertyType<float>("friction");
                if (frictionProp) {
                    friction = frictionProp->data();
                }
                entry.rigidBody->setFriction(friction);

                m_propellingEntries.erase(entity->getIntId());

            }
        }
    }
}

void PhysicalDomain::applyTransform(LocatedEntity& entity, const WFMath::Quaternion& orientation, const WFMath::Point<3>& pos, const WFMath::Vector<3>& velocity)
{
    auto I = m_entries.find(entity.getIntId());
    assert(I != m_entries.end());
    BulletEntry* entry = I->second;
    applyVelocity(*entry, velocity);
    if (entry->rigidBody) {
        if (orientation.isValid() || pos.isValid()) {
            bool hadChange = false;
            if (orientation.isValid() && !orientation.isEqualTo(entity.m_location.m_orientation)) {
                debug_print("PhysicalDomain::new orientation " << entity.describeEntity() << " " << orientation);
                btTransform& transform = entry->rigidBody->getWorldTransform();
                transform.setRotation(Convert::toBullet(orientation));
                entry->rigidBody->setWorldTransform(transform);
                entity.m_location.m_orientation = orientation;
                entity.resetFlags(entity_orient_clean);
                hadChange = true;
            }
            if (pos.isValid()) {
                WFMath::Point<3> oldPos = entity.m_location.m_pos;
                applyNewPositionForEntity(entry, pos);
                if (!oldPos.isEqualTo(entity.m_location.m_pos)) {
                    entity.resetFlags(entity_pos_clean);
                    hadChange = true;
                    //Check if there previously wasn't any valid pos, and thus no valid collision instances.
                    if (entity.m_location.m_pos.isValid() && !oldPos.isValid()) {
                        short collisionMask;
                        short collisionGroup;
                        getCollisionFlagsForEntity(entity, collisionGroup, collisionMask);
                        if (entry->rigidBody) {
                            m_dynamicsWorld->addRigidBody(entry->rigidBody, collisionGroup, collisionMask);
                        }
                        if (entry->viewSphere) {
                            m_visibilityWorld->addCollisionObject(entry->viewSphere, VISIBILITY_MASK_OBSERVABLE, VISIBILITY_MASK_OBSERVER);
                        }
                        if (entry->visibilitySphere) {
                            m_visibilityWorld->addCollisionObject(entry->visibilitySphere, VISIBILITY_MASK_OBSERVER, VISIBILITY_MASK_OBSERVABLE);
                        }
                    }
                }
            }
            if (hadChange) {
                updateTerrainMod(entity);
                if (entry->rigidBody->getInvMass() != 0) {
                    entry->rigidBody->activate();
                }
            }
        }
    }
}

void PhysicalDomain::refreshTerrain(const std::vector<WFMath::AxisBox<2>>& areas)
{
    //Schedule dirty terrain areas for update in processDirtyTerrainAreas() which is called for each tick.
    m_dirtyTerrainAreas.insert(m_dirtyTerrainAreas.end(), areas.begin(), areas.end());
}

void PhysicalDomain::processDirtyTerrainAreas()
{

    if (!m_terrain) {
        m_dirtyTerrainAreas.clear();
        return;
    }

    if (m_dirtyTerrainAreas.empty()) {
        return;
    }

    std::set<Mercator::Segment*> dirtySegments;
    for (auto& area : m_dirtyTerrainAreas) {
        m_terrain->processSegments(area, [&](Mercator::Segment& s, int, int) { dirtySegments.insert(&s); });
    }
    m_dirtyTerrainAreas.clear();

    float friction = 1.0f;
    const Property<float>* frictionProp = m_entity.getPropertyType<float>("friction");
    if (frictionProp) {
        friction = frictionProp->data();
    }

    float worldHeight = m_entity.m_location.bBox().highCorner().z() - m_entity.m_location.bBox().lowCorner().z();

    debug_print("dirty segments: " << dirtySegments.size());
    for (auto& segment : dirtySegments) {

        debug_print("rebuilding segment at x: " << segment->getXRef() << " y: " << segment->getYRef());

        buildTerrainPage(*segment, friction);

        VisibilityCallback callback;

        callback.m_collisionFilterGroup = COLLISION_MASK_TERRAIN;
        callback.m_collisionFilterMask = COLLISION_MASK_PHYSICAL | COLLISION_MASK_NON_PHYSICAL;

        auto area = segment->getRect();
        WFMath::Vector<2> size = area.highCorner() - area.lowCorner();

        btBoxShape boxShape(btVector3(size.x() * 0.5f, worldHeight, size.y() * 0.5f));
        btCollisionObject collObject;
        collObject.setCollisionShape(&boxShape);
        auto center = area.getCenter();
        collObject.setWorldTransform(btTransform(btQuaternion::getIdentity(), btVector3(center.x(), 0, -center.y())));
        m_dynamicsWorld->contactTest(&collObject, callback);

        debug_print("Matched " << callback.m_entries.size() << " entries");
        for (BulletEntry* entry : callback.m_entries) {
            debug_print("Adjusting " << entry->entity->describeEntity());
            Anonymous anon;
            anon->setId(entry->entity->getId());
            std::vector<double> posList;
            addToEntity(entry->entity->m_location.m_pos, posList);
            anon->setPos(posList);
            Move move;
            move->setTo(entry->entity->getId());
            move->setFrom(entry->entity->getId());
            move->setArgs1(anon);
            entry->entity->sendWorld(move);
        }

    }
}

void PhysicalDomain::sendMoveSight(BulletEntry& entry, bool posChange, bool velocityChange, bool orientationChange, bool angularChange)
{

    LocatedEntity& entity = *entry.entity;
    Location& lastSentLocation = entry.lastSentLocation;

    if (!entry.observingThis.empty()) {
        bool shouldSendOp = false;
        Anonymous move_arg;
        if (velocityChange) {
            ::addToEntity(entity.m_location.velocity(), move_arg->modifyVelocity());
            shouldSendOp = true;
            lastSentLocation.m_velocity = entity.m_location.velocity();
        }
        if (angularChange) {
            move_arg->setAttr("angular", entity.m_location.m_angularVelocity.toAtlas());
            shouldSendOp = true;
            lastSentLocation.m_angularVelocity = entity.m_location.m_angularVelocity;
        }
        if (orientationChange) {
            move_arg->setAttr("orientation", entity.m_location.orientation().toAtlas());
            shouldSendOp = true;
            lastSentLocation.m_orientation = entity.m_location.m_orientation;
        }
        if (posChange) {
            ::addToEntity(entity.m_location.pos(), move_arg->modifyPos());
            shouldSendOp = true;
            lastSentLocation.m_pos = entity.m_location.m_pos;
        }

        if (shouldSendOp) {
            Move m;
            move_arg->setId(entity.getId());
            if (debug_flag) {
                debug_print("Sending move op.");
                if (entity.m_location.velocity().isValid()) {
                    debug_print("new velocity: " << entity.m_location.velocity() << " " << entity.m_location.velocity().mag());
                }
            }

            //entity.m_location.addToEntity(move_arg);

            m->setArgs1(move_arg);
            m->setFrom(entity.getId());
            m->setTo(entity.getId());
            double seconds = BaseWorld::instance().getTime();
            m->setSeconds(seconds);


            for (BulletEntry* observer : entry.observingThis) {
                Sight s;
                s->setArgs1(m);
                s->setTo(observer->entity->getId());
                s->setFrom(entity.getId());
                s->setSeconds(seconds);

                entity.sendWorld(s);
            }


            //entry.lastSentLocation = entity.m_location;
        }
    }

}

void PhysicalDomain::processMovedEntity(BulletEntry& bulletEntry)
{
    LocatedEntity& entity = *bulletEntry.entity;
    Location& lastSentLocation = bulletEntry.lastSentLocation;
    const Location& location = entity.m_location;

    bool orientationChange = location.m_orientation.isValid() && !location.m_orientation.isEqualTo(lastSentLocation.m_orientation, 0.1f);


    if (false) {
        sendMoveSight(bulletEntry, true, true, true, true);
    } else {

        bool velocityChange = false;

        if (entity.m_location.m_velocity.isValid()) {
            bool hadValidVelocity = lastSentLocation.m_velocity.isValid();
            //Send an update if either the previous velocity was invalid, or any of the velocity components have changed enough, or if either the new or the old velocity is zero.
            if (!hadValidVelocity) {
                debug_print("No previous valid velocity " << entity.describeEntity() << " " << lastSentLocation.m_velocity);
                velocityChange = true;
                lastSentLocation.m_velocity = entity.m_location.m_velocity;
            } else {
                bool xChange = !fuzzyEquals(location.m_velocity.x(), lastSentLocation.m_velocity.x(), 0.01f);
                bool yChange = !fuzzyEquals(location.m_velocity.y(), lastSentLocation.m_velocity.y(), 0.01f);
                bool zChange = !fuzzyEquals(location.m_velocity.z(), lastSentLocation.m_velocity.z(), 0.01f);
                bool hadZeroVelocity = lastSentLocation.m_velocity.isEqualTo(WFMath::Vector<3>::ZERO());
                if (xChange || yChange || zChange) {
                    debug_print("Velocity changed " << entity.describeEntity() << " " << location.m_velocity);
                    velocityChange = true;
                    lastSentLocation.m_velocity = entity.m_location.velocity();
                } else if (entity.m_location.m_velocity.isEqualTo(WFMath::Vector<3>::ZERO()) && !hadZeroVelocity) {
                    debug_print("Old or new velocity zero " << entity.describeEntity() << " " << location.m_velocity);
                    velocityChange = true;
                    lastSentLocation.m_velocity = entity.m_location.velocity();
                }
            }
        }
        bool angularChange = false;

        if (entity.m_location.m_angularVelocity.isValid()) {
            bool hadZeroAngular = lastSentLocation.m_angularVelocity.isEqualTo(WFMath::Vector<3>::ZERO());
            angularChange = !fuzzyEquals(lastSentLocation.m_angularVelocity, location.m_angularVelocity, 0.01f);
            if (!angularChange && entity.m_location.m_angularVelocity.isEqualTo(WFMath::Vector<3>::ZERO()) && !hadZeroAngular) {
                debug_print("Angular changed " << entity.describeEntity() << " " << location.m_angularVelocity);
                angularChange = true;
                lastSentLocation.m_angularVelocity = entity.m_location.m_angularVelocity;
            }
        }
        if (velocityChange || orientationChange || angularChange) {
            sendMoveSight(bulletEntry, true, velocityChange, orientationChange, angularChange);
            lastSentLocation.m_pos = entity.m_location.m_pos;
        }

    }

    updateTerrainMod(entity);
}

void PhysicalDomain::tick(double tickSize, OpVector& res)
{
    processDirtyTerrainAreas();

//    CProfileManager::Reset();
//    CProfileManager::Increment_Frame_Counter();

    auto start = std::chrono::high_resolution_clock::now();
    //Step simulations with 60 hz.
    m_dynamicsWorld->stepSimulation((float) tickSize, static_cast<int>(60 * tickSize));

    std::stringstream ss;
    ss << "Tick: " << (tickSize * 1000) << " ms Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000.f) << " ms";
    debug_print(ss.str());

    //CProfileManager::dumpAll();

    //Don't do visibility checks each tick; instead use m_visibilityCheckCountdown to count down to next
    m_visibilityCheckCountdown -= tickSize;
    if (m_visibilityCheckCountdown <= 0) {
        updateVisibilityOfDirtyEntities(res);
        m_visibilityCheckCountdown = VISIBILITY_CHECK_INTERVAL_SECONDS;
    }

    //Check all entities that moved this tick.
    for (BulletEntry* entry : m_movingEntities) {
        //Check if the entity also moved last tick.
        if (m_lastMovingEntities.find(entry) == m_lastMovingEntities.end()) {
            //Didn't move before
            processMovedEntity(*entry);
        } else {
            processMovedEntity(*entry);
            //Erase from last moving entities, so we can find those that moved last tick, but not this.
            m_lastMovingEntities.erase(entry);
        }
    }

    for (BulletEntry* entry : m_lastMovingEntities) {
        //Stopped moving
        if (entry->entity->m_location.m_angularVelocity.isValid()) {
            entry->entity->m_location.m_angularVelocity.zero();
        }
        if (entry->entity->m_location.m_velocity.isValid()) {
            debug_print("Stopped moving " << entry->entity->describeEntity());
            entry->entity->m_location.m_velocity.zero();
        }
        processMovedEntity(*entry);
    }

    //Stash those entities that moved this tick for checking next tick.
    std::swap(m_movingEntities, m_lastMovingEntities);
    m_movingEntities.clear();
}

bool PhysicalDomain::getTerrainHeight(float x, float y, float& height) const
{
    if (m_terrain) {
        Mercator::Segment* s = m_terrain->getSegmentAtPos(x, y);
        if (s != 0 && !s->isValid()) {
            s->populate();
        }
        return m_terrain->getHeight(x, y, height);
    }
    return false;
}

