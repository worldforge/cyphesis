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

#include "rules/Domain.h"
#include "rules/Location.h"
#include "ModeProperty.h"
#include "rules/PhysicalProperties.h"

#include <sigc++/connection.h>

#include <LinearMath/btVector3.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <map>
#include <unordered_map>
#include <tuple>
#include <array>
#include <set>
#include <unordered_set>

namespace Mercator {
    class Segment;

    class Terrain;

    class TerrainMod;
}

class btDefaultCollisionConfiguration;

class btCollisionDispatcher;

class btBroadphaseInterface;

class btCollisionWorld;

class btSequentialImpulseConstraintSolver;

class PhysicalWorld;

class btRigidBody;

class btCollisionShape;

class btSphereShape;

class btCollisionObject;

class btAxisSweep3;

class PropelProperty;

/**
 * @brief A regular physical domain, behaving very much like the real world.
 *
 * Things move using physical rules, and sights are calculated using line of sight.
 *
 */
class PhysicalDomain : public Domain
{
    public:
        static int s_processTimeUs;

        explicit PhysicalDomain(LocatedEntity& entity);

        ~PhysicalDomain() override;

        bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override;

        void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override;

        std::vector<LocatedEntity*> getObservingEntitiesFor(const LocatedEntity& observedEntity) const override;

        void addEntity(LocatedEntity& entity) override;

        void removeEntity(LocatedEntity& entity) override;

        void applyTransform(LocatedEntity& entity, const TransformData& transformData,
                            std::set<LocatedEntity*>& transformedEntities) override;

        void refreshTerrain(const std::vector<WFMath::AxisBox<2>>& areas) override;

        void toggleChildPerception(LocatedEntity& entity) override;

        bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override;

        void installDelegates(LocatedEntity& entity, const std::string& propertyName) override;

        HandlerResult operation(LocatedEntity& e, const Operation& op, OpVector& res) override;

        void tick(double t, OpVector& res);

        std::vector<CollisionEntry> queryCollision(const WFMath::Ball<3>& sphere) const override;

        boost::optional<std::function<void()>> observeCloseness(LocatedEntity& entity1, LocatedEntity& entity2, double reach, std::function<void()> callback) override;

        void removed() override;

    protected:

        struct PhysicalMotionState;

        struct VisibilityPairCallback;

        struct WaterCollisionCallback;

        struct ClosenessObserverEntry;

        struct PhysicalData
        {
            Point3D pos;
            Vector3D velocity;
            Vector3D angularVelocity;
            Quaternion orientation;
        };

        struct BulletEntry
        {
            enum class VisibilityQueueOperationType
            {
                    Add,
                    Remove
            };
            LocatedEntity& entity;
            /**
             * The following properties are all "physical" properties which only make sense in this domain.
             * The current setup is to have them as regular properties, but in reality they are "owned" by this domain.
             */
            PositionProperty& positionProperty;
            VelocityProperty& velocityProperty;
            AngularVelocityProperty& angularVelocityProperty;
            OrientationProperty& orientationProperty;

            /**
             * We keep a copy of the bbox here for quick lookup.
             */
            WFMath::AxisBox<3> bbox;

            /**
             * An entity that's "solid" will react physically with other entities.
             * For now it will suffice with a simple model where things are either solid or not.
             * Depending on requirements it might be worth looking into using a more complex system with filters. Though that might perhaps just make things too complex.
             */
            bool isSolid;

            std::shared_ptr<btCollisionShape> collisionShape;
            std::unique_ptr<btCollisionObject> collisionObject;
            sigc::connection propertyUpdatedConnection;

            PhysicalData lastSentLocation;
            std::unique_ptr<PhysicalMotionState> motionState;

            std::unique_ptr<btCollisionObject> visibilitySphere;
            std::unique_ptr<btSphereShape> visibilityShape;
            std::unique_ptr<btCollisionObject> viewSphere;
            std::unique_ptr<btSphereShape> viewShape;

            /**
             * Set of entries which are observing by this.
             */
            std::set<BulletEntry*> observedByThis;
            /**
             * Changes to the entities that are observed by this are recorded here, and then
             * moved to observedByThis along with generating Appear and Disappear ops.
             */
            std::vector<std::pair<BulletEntry*, VisibilityQueueOperationType>> observedByThisChanges;

            /**
             * Set of entries which are observing this.
             */
            std::set<BulletEntry*> observingThis;
            /**
             * Changes to the entities that are observing this are recorded here, and then
             * moved to observedByThis along with generating Appear and Disappear ops.
             */
            std::vector<std::pair<BulletEntry*, VisibilityQueueOperationType>> observingThisChanges;

            btVector3 centerOfMassOffset;

            /**
             * Max speed when on solid ground, derived from the "speed_ground" property and cached for performance.
             */
            double speedGround;
            /**
             * Max speed when in water (swimming), derived from the "speed_water" property and cached for performance.
             */
            double speedWater;
            /**
             * Max speed when flying, derived from the "speed_flight" property and cached for performance.
             */
            double speedFlight;

            /**
             * The mode of the entity, as known by the physics system.
             */
            ModeProperty::Mode mode;

            /**
             * Set to true if the mode has been changed in a tick, which should be communicated in a Move op.
             */
            bool modeChanged = false;

            /**
             * Set to true if the entry already has been added to m_visibilityRecalculateQueue
             */
            bool markedForVisibilityRecalculation = false;

            /**
             * Set to true if the entry has been added to m_movingEntities
             */
            bool addedToMovingList = false;

            /**
             * Set to true if the entity moved the current frame. This is used to detect entities that stopped or started moving.
             */
            bool markedAsMovingThisFrame = false;

            /**
             * Set to true if the entity moved the last frame. This is used to detect entities that stopped or started moving.
             */
            bool markedAsMovingLastFrame = false;

            /**
             * A set of entities which are planted on this. They move along.
             */
            std::set<BulletEntry*> attachedEntities;

            /**
             * Keeps track of whether the entity is actively jumping. If so we shouldn't try to clamp it to the ground.
             */
            bool isJumping = false;

            std::set<ClosenessObserverEntry*> closenessObservations;

            /**
             * Keeps track of last received transform. This allows us to quickly check if we should mark the entity as dirty or not
             * (which we only do if position or orientation has changed)
             */
            btTransform lastTransform{};

            /**
             * Set to a bullet entry for a water body if the entry and the water body are close enough for the broadphase to consider them.
             * This does not mean that the entity actually is contained in the water; this needs to be checked closer through collision checks.
             *
             * When the entity is moved out of the water body this is set to the bullet entry itself (instead of setting it to null). This way
             * we have a way of knowing that the entity has been moved out.
             */
            BulletEntry* waterNearby = nullptr;

            double step_factor = 0;

            /**
             * Contains references to properties that control the entity, if they exists.
             */
            struct
            {
                const PropelProperty* propelProperty;
                const QuaternionProperty* directionProperty;
                const Vector3Property* destinationProperty;
            } control;

        };

        struct TerrainEntry
        {
            std::unique_ptr<std::array<btScalar, 65 * 65>> data;
            std::unique_ptr<btRigidBody> rigidBody;
            std::unique_ptr<btCollisionShape> shape;
        };


        struct ClosenessObserverEntry
        {
            BulletEntry* reacher;
            BulletEntry* target;
            double reach;
            /**
             * Callback to call when entries no longer are close.
             */
            std::function<void()> callback;
        };

        std::map<ClosenessObserverEntry*, std::unique_ptr<ClosenessObserverEntry>> m_closenessObservations;

        std::unordered_map<long, std::unique_ptr<BulletEntry>> m_entries;

        std::vector<BulletEntry*> m_movingEntities;

        /**
         * Contains entities which needs to have their visibility recalculated, either because they moved or they changed size.
         */
        std::vector<BulletEntry*> m_visibilityRecalculateQueue;

        /**
         * Keeps track of all water bodies, and the entities that currently are near them (as determined by the broadphase proxy).
         * The entities contained in the set are thus _possibly_ contained in the water, but not necessarily. The main reason
         * for keeping this structure is for when a water body is moved (which would be _very_ seldom).
         * In those cases we also need to move the entities that are contained, which is what the set is for.
         * A std::set is an expensive container, but as we anticipate entities rarely moveing in and out of water, and even more
         * rarely a water entity being moved instead, we can probably afford it in this case.
         */
        //std::map<BulletEntry*, std::set<BulletEntry*>> m_waterBodies;

        std::vector<WFMath::AxisBox<2>> m_dirtyTerrainAreas;

        struct TerrainModEntry
        {
            WFMath::Point<3> modPos;
            WFMath::Quaternion modOrientation;
            WFMath::AxisBox<2> area;
        };
        std::map<long, TerrainModEntry> m_terrainMods;


        struct PropelEntry
        {
            btRigidBody* rigidBody;
            BulletEntry* bulletEntry;
            btVector3 velocity;
            float stepHeight;
        };
        /**
         * @brief A map of all entities that currently are self-propelling.
         *
         * Each tick the propel force will be applied to these entities.
         */
        std::map<long, PropelEntry> m_propellingEntries;

        /**
         * A set of entities that have their "propel" updated and needs to be processed next tick.
         */
        std::set<BulletEntry*> m_propelUpdateQueue;
        /**
         * A set of entities that have their "direction" updated and needs to be processed next tick.
         */
        std::set<BulletEntry*> m_directionUpdateQueue;

        /**
         * A set of entities with "destination" set. These needs to be processed each tick to see the propel property needs to be updated.
         */
        std::vector<BulletEntry*> m_entriesWithDestination;


        /**
         * Keeps track of all stepping entries, i.e. those we want clamped to the ground.
         */
        std::set<BulletEntry*> m_steppingEntries;

        /**
         * Struct used to pass information on to the tick callbacks.
         */
        struct WorldInfo
        {
            std::map<long, PropelEntry>* propellingEntries;
            std::set<BulletEntry*>* steppingEntries;
        };

        WorldInfo mWorldInfo;

        std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
        std::unique_ptr<btCollisionDispatcher> m_dispatcher;
        std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraintSolver;
        std::unique_ptr<btBroadphaseInterface> m_broadphase;
        std::unique_ptr<PhysicalWorld> m_dynamicsWorld;

        std::unique_ptr<btOverlappingPairCallback> m_visibilityPairCallback;
        std::unique_ptr<btCollisionDispatcher> m_visibilityDispatcher;
        std::unique_ptr<btAxisSweep3> m_visibilityBroadphase;
        std::unique_ptr<btCollisionWorld> m_visibilityWorld;

        sigc::connection m_propertyAppliedConnection;

        double m_visibilityCheckCountdown;

        struct
        {
            PositionProperty positionProperty;
            VelocityProperty velocityProperty;
            AngularVelocityProperty angularVelocityProperty;
            OrientationProperty orientationProperty;
        } mFakeProperties;


        BulletEntry mContainingEntityEntry;

        Mercator::Terrain* m_terrain;

        /**
         * Looks for broadphase collisions with water entities. These are candidates for later checking
         * if the entity actually is contained in the water.
         */
        std::unique_ptr<WaterCollisionCallback> m_ghostPairCallback;

        /**
         * @brief Contains all terrain segments, as height fields.
         *
         * Each segment is 65*65 points.
         */
        std::unordered_map<std::string, TerrainEntry> m_terrainSegments;

        /**
         * Contains the six planes that make out the border, which matches the bounding box of the entity to which this
         * property belongs.
         */
        std::vector<std::pair<std::unique_ptr<btRigidBody>, std::unique_ptr<btCollisionShape>>> m_borderPlanes;


        /**
         * @brief Creates borders around the domain, which prevents entities from "escaping".
         *
         * This will be done by adding planes matching the bbox of the entity to which the domain belongs.
         */
        void createDomainBorders();

        /**
         * @brief Build all terrain pages.
         */
        void buildTerrainPages();

        /**
         * @brief Builds one terrain page from a Mercator segment.
         * @param segment
         */
        TerrainEntry& buildTerrainPage(Mercator::Segment& segment);

        /**
         * Listener method for all child entities, called when their properties change.
         * @param name
         * @param prop
         * @param bulletEntry
         */
        void childEntityPropertyApplied(const std::string& name, const PropertyBase& prop, BulletEntry& bulletEntry);

        /**
         * Listener method for changes to properties on the entity to which the property belongs.
         * @param name
         * @param prop
         * @param bulletEntry
         */
        void entityPropertyApplied(const std::string& name, const PropertyBase& prop);

        float getMassForEntity(const LocatedEntity& entity) const;

        void getCollisionFlagsForEntity(const BulletEntry& entry, short& collisionGroup, short& collisionMask) const;

        void sendMoveSight(BulletEntry& bulletEntry, bool posChange, bool velocityChange, bool orientationChange, bool angularChange, bool modeChanged);

        void processMovedEntity(BulletEntry& bulletEntry, double timeSinceLastUpdate);

        void updateVisibilityOfDirtyEntities(OpVector& res);

        void updateObservedEntry(BulletEntry& entry, OpVector& res, bool generateOps = true);

        void updateObserverEntry(BulletEntry& bulletEntry, OpVector& res);

        void applyNewPositionForEntity(BulletEntry& entry, const WFMath::Point<3>& pos, bool calculatePosition = true);

        bool getTerrainHeight(float x, float y, float& height) const;

        void updateTerrainMod(const BulletEntry& entry, bool forceUpdate = false);

        void processDirtyTerrainAreas();

        void applyDestination(double tickSize, BulletEntry& entry, const PropelProperty* propelProp, const Vector3Property& destinationProp);

        void applyPropel(BulletEntry& entry, btVector3 propel);

        void calculatePositionForEntity(ModeProperty::Mode mode, BulletEntry& entry, WFMath::Point<3>& pos);

        /**
         * Called each tick to process any bodies that are moving in water.
         */
        void processWaterBodies();

        std::shared_ptr<btCollisionShape> createCollisionShapeForEntry(LocatedEntity& entity,
                                                                       const WFMath::AxisBox<3>& bbox, float mass,
                                                                       btVector3& centerOfMassOffse);

        /**
         * Transform any entities that are resting on the supplied entity.
         *
         * I.e. those that are "free" or "submerged" and are on top of the entity.
         * @param entry
         * @param posTransform
         * @param transformedEntities
         */
        void transformRestingEntities(BulletEntry& entry,
                                      const WFMath::Vector<3>& posTransform,
                                      const WFMath::Quaternion& orientationChange,
                                      std::set<LocatedEntity*>& transformedEntities);

        void plantOnEntity(BulletEntry& plantedEntry, BulletEntry* entryPlantedOn);

        void applyTransformInternal(BulletEntry& entry,
                                    const WFMath::Quaternion& orientation,
                                    const WFMath::Point<3>& pos,
                                    const WFMath::Vector<3>& impulseVelocity,
                                    std::set<LocatedEntity*>& transformedEntities,
                                    bool calculatePosition);

        Atlas::Objects::Operation::RootOperation scheduleTick(LocatedEntity& entity);

        HandlerResult tick_handler(LocatedEntity& entity, const Operation& op, OpVector& res);

        bool isWithinReach(BulletEntry& reacherEntry, BulletEntry& targetEntry, float reach, const WFMath::Point<3>& positionOnQueriedEntity) const;


        /**
         * Calculate the radius of a visibility sphere, taking both any "vis_dist" property as
         * well as the entity's size into account.
         * @param entity
         * @return
         */
        float calculateVisibilitySphereRadius(const BulletEntry& entry) const;
};

#endif /* PHYSICALDOMAIN_H_ */
