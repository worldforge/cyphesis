/*
 Copyright (C) 2020 Erik Ogenvik

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

#include <navigation/Steering.h>
#include <navigation/Awareness.h>
#include "navigation/IHeightProvider.h"
#include "rules/MemEntity.h"
#include "../TestBase.h"
#include "../TestWorld.h"


struct MemEntityExt;
//Keep track of all created things and make sure they are destroyed when the Context is destroyed.
//This is needed to avoid recursive references, where an entity refers to its children, and the children to their parent.
//This might be removed if we instead store "parent" as a simple pointer.
static std::vector<Ref<MemEntityExt>> things;

struct MemEntityExt : public MemEntity
{
    explicit MemEntityExt(long intId)
            : MemEntityExt::MemEntityExt(std::to_string(intId), intId)
    {
    }

    explicit MemEntityExt(const std::string& id, long intId)
            : MemEntity::MemEntity(id, intId)
    {
        things.emplace_back(Ref<MemEntityExt>(this));
    }

    void destroy() override
    {
        m_parent = nullptr;
        if (m_contains) {
            m_contains->clear();
        }
    }
};


double epsilon = 0.00001;

bool operator==(const Location& lhs, const Location& rhs)
{
    return lhs.m_parent == rhs.m_parent && lhs.m_pos == rhs.m_pos;
}

bool operator!=(const Location& lhs, const Location& rhs)
{
    return !(lhs == rhs);
}

void operator<<(std::ostringstream& out, const Location& rhs)
{

}

WFMath::Point<2> to2D(const WFMath::Point<3>& point)
{
    return {point.x(), point.z()};
}


struct SteeringIntegration : public Cyphesis::TestBase
{
    static long m_id_counter;

    SteeringIntegration()
    {
        ADD_TEST(SteeringIntegration::test_create);
        ADD_TEST(SteeringIntegration::test_prediction);
        ADD_TEST(SteeringIntegration::test_steering);
        ADD_TEST(SteeringIntegration::test_resolveDestination);
        ADD_TEST(SteeringIntegration::test_distance);
        ADD_TEST(SteeringIntegration::test_navigation);
    }

    void setup()
    {

    }

    void teardown()
    {
        for (auto thing : things) {
            thing->destroy();
        }
        things.clear();
    }

    void test_create()
    {
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_bbox = {{-1, -1, -1},
                                {1,  1,  1}};
        Steering steering(*avatarEntity);
    }

    void test_resolveDestination()
    {
        Ref<MemEntity> worldEntity(new MemEntityExt("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_transform.pos = {0, 0, 0};
        avatarEntity->m_bbox = {{-1, 0, -1},
                                {1,  1, 1}};
        Ref<MemEntity> otherEntity(new MemEntityExt("2", 2));
        otherEntity->m_transform.pos = {10, 0, 0};
        Ref<MemEntity> outOfWorldEntity(new MemEntityExt("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntityExt("4", 4));
        Ref<MemEntity> otherChildEntity(new MemEntityExt("5", 5));

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        avatarEntity->addChild(*avatarChildEntity);
        otherEntity->addChild(*otherChildEntity);
        Steering steering(*avatarEntity);

        //Test with both the location, as well as the entity itself.
        ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.resolvePosition(0, {EntityLocation(otherEntity)}).position);

        //Location should be null if we only specify a point
        ASSERT_EQUAL(0, steering.resolvePosition(0, {EntityLocation{otherEntity}}).radius)
        //Location should be set if we specify a new Location with a parent (even if it's with an invalid position).
//        ASSERT_EQUAL(otherEntity->m_location, *steering.resolvePosition(0, {EntityLocation(otherEntity)}).location);

        // ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.resolvePosition(0, {otherChildEntity->m_location}).position);
        ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.resolvePosition(0, {EntityLocation(otherChildEntity)}).position);

        //The resolved location of "otherChildEntity" should be it's parent's location.
        //ASSERT_EQUAL(otherEntity->m_location, *steering.resolvePosition(0, {otherChildEntity->m_location}).location);
        //ASSERT_EQUAL(otherEntity->m_location, *steering.resolvePosition(0, {EntityLocation(otherChildEntity)}).location);

//        ASSERT_EQUAL(WFMath::Point<3>(0, 0, 0), steering.resolvePosition(0, {avatarChildEntity->m_location}).position);
        ASSERT_EQUAL(WFMath::Point<3>(0, 0, 0), steering.resolvePosition(0, {EntityLocation(avatarChildEntity)}).position);

//        ASSERT_EQUAL(avatarEntity->m_location, *steering.resolvePosition(0, {avatarChildEntity->m_location}).location);
//        ASSERT_EQUAL(avatarEntity->m_location, *steering.resolvePosition(0, {EntityLocation(avatarChildEntity)}
//
//        ).location);
    }

    void test_steering()
    {
        Ref<MemEntity> worldEntity(new MemEntityExt("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_transform.pos = {0, 0, 0};
        avatarEntity->m_bbox = {{-1, 0, -1},
                                {1,  1, 1}};
        avatarEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto avatarHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(avatarEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(1.41421, avatarHorizontalRadius, epsilon);
        Ref<MemEntity> otherEntity(new MemEntityExt("2", 2));
        otherEntity->m_transform.pos = {10, 0, 0};
        otherEntity->m_bbox = {{-2, 0, -2},
                               {2,  3, 2}};
        otherEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto otherHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(otherEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(2.828425, otherHorizontalRadius, epsilon);
        Ref<MemEntity> outOfWorldEntity(new MemEntityExt("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntityExt("4", 4));
        Ref<MemEntity> obstacleEntity(new MemEntityExt("5", 5));
        obstacleEntity->m_bbox = {{-1, 0, -1},
                                  {1,  1, 1}};
        obstacleEntity->m_transform.pos = {5, 0, 0};
        obstacleEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        worldEntity->addChild(*obstacleEntity);
        avatarEntity->addChild(*avatarChildEntity);
        Steering steering(*avatarEntity);
        WFMath::AxisBox<3> extent = {{-64, -64, -64},
                                     {64,  64,  64}};

        static int tileSize = 64;
        struct : public IHeightProvider
        {
            void blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const override
            {
                heights.resize(tileSize * tileSize, 0);
            }

        } heightProvider;

        Awareness awareness(*worldEntity, avatarHorizontalRadius, 2, 0.5, heightProvider, extent, tileSize);
        steering.setAwareness(&awareness);
        auto rebuildAllTilesFn = [&]() {
            while (true) {
                if (awareness.rebuildDirtyTile() == 0) {
                    break;
                }
            }
        };
        //The other entity will be added as a dynamic one.
        awareness.addEntity(*avatarEntity, *otherEntity, true);

        steering.startSteering();
        steering.setDestination({EntityLocation(otherEntity), Steering::MeasureType::EDGE, Steering::MeasureType::EDGE, 0}, 0);
        //Set to ten meters per second, so it should take one second to reach the other entity.
        steering.setDesiredSpeed(10);
        rebuildAllTilesFn();

        auto result = steering.update(0);
        //Since it's a straight line we can move directly to destination.
        ASSERT_TRUE(result.destination.isValid());
        ASSERT_EQUAL(result.destination, otherEntity->m_transform.pos);
        ASSERT_FUZZY_EQUAL(1.0, *result.timeToNextWaypoint, epsilon);
        ASSERT_EQUAL(WFMath::Vector<3>(10, 0, 0), result.direction);

        //Set location so that the entity should arrive in one second
        avatarEntity->m_movement.velocity.data = {10, 0, 0};
        result = steering.update(1.0);
        ASSERT_FALSE(result.destination.isValid());
        ASSERT_FALSE(result.timeToNextWaypoint);
        ASSERT_TRUE(result.direction.isValid());
        ASSERT_EQUAL(WFMath::Vector<3>::ZERO(), result.direction);

        //Add the obstacle entity and make sure that we now divert around it.
        awareness.addEntity(*avatarEntity, *obstacleEntity, false);
        rebuildAllTilesFn();
        result = steering.update(0);
        //We can't move directly to destination this time
        ASSERT_FALSE(result.destination.isValid());

    }

    void test_prediction()
    {
        Ref<MemEntity> worldEntity(new MemEntityExt("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_transform.pos = {0, 0, 0};
        avatarEntity->m_bbox = {{-1, 0, -1},
                                {1,  1, 1}};
        avatarEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto avatarHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(avatarEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(1.41421, avatarHorizontalRadius, epsilon);
        Ref<MemEntity> otherEntity(new MemEntityExt("2", 2));
        otherEntity->m_transform.pos = {10, 0, 0};
        otherEntity->m_bbox = {{-2, 0, -2},
                               {2,  3, 2}};
        otherEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto otherHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(otherEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(2.828425, otherHorizontalRadius, epsilon);
        Ref<MemEntity> outOfWorldEntity(new MemEntityExt("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntityExt("4", 4));

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        avatarEntity->addChild(*avatarChildEntity);
        Steering steering(*avatarEntity);
        WFMath::AxisBox<3> extent = {{-64, -64, -64},
                                     {64,  64,  64}};

        static int tileSize = 64;
        struct : public IHeightProvider
        {
            void blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const override
            {
                heights.resize(tileSize * tileSize, 0);
            }

        } heightProvider;

        Awareness awareness(*worldEntity, avatarHorizontalRadius, 2, 0.5, heightProvider, extent, tileSize);
        steering.setAwareness(&awareness);
        auto rebuildAllTilesFn = [&]() {
            while (true) {
                if (awareness.rebuildDirtyTile() == 0) {
                    break;
                }
            }
        };
        //The other entity will be added as a dynamic one.
        awareness.addEntity(*avatarEntity, *otherEntity, true);
        rebuildAllTilesFn();

        //Set avatar velocity to 10 meters per second
        avatarEntity->m_movement.velocity.data = {10, 0, 0};

        ASSERT_EQUAL(WFMath::Point<3>(0, 0, 0), steering.getCurrentAvatarPosition(0));
        ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.getCurrentAvatarPosition(1.0));
        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0.0, *steering.distanceTo(1.0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        //Overshoot the target
        ASSERT_FUZZY_EQUAL(20.0, *steering.distanceTo(3.0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0.0, *steering.distanceTo(0, EntityLocation(avatarChildEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0.0, *steering.distanceTo(1.0, EntityLocation(avatarChildEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0.0, *steering.distanceTo(3.0, EntityLocation(avatarChildEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        steering.setDestination({EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER, 0.5}, 0);
        rebuildAllTilesFn();
        ASSERT_FALSE(steering.isAtCurrentDestination(0));
        ASSERT_TRUE(steering.isAtCurrentDestination(1.0));

        //Set velocity of the other entity too
        otherEntity->m_movement.velocity.data = {10, 0, 0};
        //This should be true, since we supplied a fixed destination
        ASSERT_TRUE(steering.isAtCurrentDestination(1.0));
        ASSERT_FUZZY_EQUAL(0.0, *steering.distanceTo(1.0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);

        steering.setDestination({EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER, 0.5}, 0);
        rebuildAllTilesFn();
        //This should be false since we're now tracking the other entity (and thus take velocity into account).
        ASSERT_FALSE(steering.isAtCurrentDestination(1.0));
        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(1.0, EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);

    }

    void test_distance()
    {
        Ref<MemEntity> worldEntity(new MemEntityExt("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_transform.pos = {0, 0, 0};
        avatarEntity->m_bbox = {{-1, 0, -1},
                                {1,  1, 1}};
        auto avatarHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(avatarEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(1.41421, avatarHorizontalRadius, epsilon);
        Ref<MemEntity> otherEntity(new MemEntityExt("2", 2));
        otherEntity->m_transform.pos = {10, 0, 0};
        otherEntity->m_bbox = {{-2, 0, -2},
                               {2,  3, 2}};
        auto otherHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(otherEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(2.828425, otherHorizontalRadius, epsilon);
        Ref<MemEntity> outOfWorldEntity(new MemEntityExt("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntityExt("4", 4));

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        avatarEntity->addChild(*avatarChildEntity);
        Steering steering(*avatarEntity);

        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        //Also test with setting the destination to the entity itself
        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(10.0 - otherHorizontalRadius, *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::EDGE), epsilon);
        //If we specify the destination as a point the "EDGE" measurement on the destination won't have any effect.
        ASSERT_FUZZY_EQUAL(10.0 - avatarHorizontalRadius,
                           *steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);
        //But if we specify the destination as the entity itself the "EDGE" measurement on the destination will have an effect.
        ASSERT_FUZZY_EQUAL(10.0 - otherHorizontalRadius - avatarHorizontalRadius,
                           *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);


        //Move other away
        otherEntity->m_transform.pos = {20, 0, 0};
        ASSERT_FUZZY_EQUAL(20.0, *steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(20.0, *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        //Also test with setting the destination to the entity itself
        ASSERT_FUZZY_EQUAL(20.0 - otherHorizontalRadius, *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::CENTER, Steering::MeasureType::EDGE), epsilon);
        ASSERT_FUZZY_EQUAL(20.0 - otherHorizontalRadius - avatarHorizontalRadius,
                           *steering.distanceTo(0, EntityLocation(otherEntity), Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);
        //Move avatar closer
        avatarEntity->m_transform.pos = {10, 0, 0};
        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);

        //Test invalid positions
        ASSERT_FALSE(steering.distanceTo(0, EntityLocation{outOfWorldEntity->m_parent, outOfWorldEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER))
        ASSERT_FALSE(steering.distanceTo(0, EntityLocation{outOfWorldEntity->m_parent, outOfWorldEntity->m_transform.pos}, Steering::MeasureType::EDGE, Steering::MeasureType::CENTER))
        ASSERT_FALSE(steering.distanceTo(0, EntityLocation{outOfWorldEntity->m_parent, outOfWorldEntity->m_transform.pos}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE))

        otherEntity->m_transform.pos = {};
        ASSERT_FALSE(steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        otherEntity->m_transform.pos = {20, 0, 0};
        avatarEntity->m_transform.pos = {};
        ASSERT_FALSE(steering.distanceTo(0, EntityLocation{otherEntity->m_parent, otherEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        avatarEntity->m_transform.pos = {10, 0, 0};

        //The distance to the avatar child entity should be 0.
        ASSERT_TRUE(steering.distanceTo(0, EntityLocation{avatarChildEntity->m_parent, avatarChildEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, EntityLocation{avatarChildEntity->m_parent, avatarChildEntity->m_transform.pos}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER),
                           epsilon);
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, EntityLocation{avatarChildEntity->m_parent, avatarChildEntity->m_transform.pos}, Steering::MeasureType::EDGE, Steering::MeasureType::CENTER),
                           epsilon);
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, EntityLocation{avatarChildEntity->m_parent, avatarChildEntity->m_transform.pos}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE),
                           epsilon);
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, EntityLocation(avatarChildEntity), Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);

    }

    void test_navigation()
    {

        Ref<MemEntity> worldEntity(new MemEntityExt("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntityExt("1", 1));
        avatarEntity->m_transform.pos = {0, 0, 0};
        avatarEntity->m_bbox = {{-1, 0, -1},
                                {1,  1, 1}};
        avatarEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto avatarHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(avatarEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(1.41421, avatarHorizontalRadius, epsilon);
        Ref<MemEntity> otherEntity(new MemEntityExt("2", 2));
        otherEntity->m_transform.pos = {10, 0, 0};
        otherEntity->m_bbox = {{-2, 0, -2},
                               {2,  3, 2}};
        otherEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();
        auto otherHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(otherEntity->m_bbox));
        ASSERT_FUZZY_EQUAL(2.828425, otherHorizontalRadius, epsilon);
        Ref<MemEntity> outOfWorldEntity(new MemEntityExt("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntityExt("4", 4));
        Ref<MemEntity> obstacleEntity(new MemEntityExt("5", 5));
        obstacleEntity->m_bbox = {{-1, 0, -1},
                                  {1,  2, 1}};
        obstacleEntity->m_transform.pos = {0, 0, 0};
        obstacleEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();

        Ref<MemEntity> smallObstacleEntity(new MemEntityExt("6", 6));
        smallObstacleEntity->m_transform.pos = {0, 0, 0};
        smallObstacleEntity->m_bbox = {{0.2, 0,   0.2},
                                       {0.2, 0.2, 0.2}};
        smallObstacleEntity->m_transform.orientation = WFMath::Quaternion::IDENTITY();


        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        worldEntity->addChild(*obstacleEntity);
        worldEntity->addChild(*smallObstacleEntity);
        avatarEntity->addChild(*avatarChildEntity);
        Steering steering(*avatarEntity);
        WFMath::AxisBox<3> extent = {{-64, -64, -64},
                                     {64,  64,  64}};

        static int tileSize = 64;
        struct : public IHeightProvider
        {
            void blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const override
            {
                heights.resize(tileSize * tileSize, 0);
            }

        } heightProvider;

        Awareness awareness(*worldEntity, avatarHorizontalRadius, 2, 0.5, heightProvider, extent, tileSize);
        steering.setAwareness(&awareness);
        auto rebuildAllTilesFn = [&]() {
            while (true) {
                if (awareness.rebuildDirtyTile() == 0) {
                    break;
                }
            }
        };
        //The other entity will be added as a dynamic one.
        awareness.addEntity(*avatarEntity, *otherEntity, true);

        double desiredDistance = 0.5;

        steering.setDestination({{EntityLocation(otherEntity)}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE, desiredDistance}, 0);
        auto result = steering.updatePath(0);
        ASSERT_EQUAL(-1, result); //Could not find any poly since we haven't built any navmeshes.
        rebuildAllTilesFn();
        ASSERT_FALSE(steering.isAtCurrentDestination(0));

        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(to2D(otherEntity->m_transform.pos), to2D(steering.getPath()[0]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Move the avatar closer
        avatarEntity->m_transform.pos.x() = 5;
        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(to2D(otherEntity->m_transform.pos), to2D(steering.getPath()[0]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Move the destination entity away
        otherEntity->m_transform.pos.x() = 15;
        steering.requestUpdate();
        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(to2D(otherEntity->m_transform.pos), to2D(steering.getPath()[0]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Move the avatar entity directly over the destination entity
        avatarEntity->m_transform.pos = otherEntity->m_transform.pos;
        steering.requestUpdate();
        result = steering.updatePath(0);
        ASSERT_EQUAL(0, result); //Should be no vert since we're exactly at destination
        ASSERT_TRUE(steering.isAtCurrentDestination(0))

        //Move the avatar entity adjacent to the destination, but within required distance
        avatarEntity->m_transform.pos.x() -= 3.0;
        steering.requestUpdate();
        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(to2D(otherEntity->m_transform.pos), to2D(steering.getPath()[0]))
        ASSERT_TRUE(steering.isAtCurrentDestination(0))

        //Move the avatar to origo, place the obstacleEntity entity besides it, and plot a path "through" the obstacleEntity entity to a position beyond it.
        //The path should go around the other entity
        avatarEntity->m_transform.pos = {0, 0, 0};
        obstacleEntity->m_transform.pos = {10, 0, 0};

        awareness.addEntity(*avatarEntity, *obstacleEntity, false);
        steering.setDestination({{EntityLocation(worldEntity, {20, 0, 0})}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER, desiredDistance}, 0);
        rebuildAllTilesFn();

        ASSERT_FALSE(steering.isAtCurrentDestination(0));

        result = steering.updatePath(0);
        ASSERT_EQUAL(3, result); //Should be three verts since we need to divert twice
        ASSERT_EQUAL(WFMath::Point<2>(20, 0), to2D(steering.getPath()[2]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Move the obstacle entity away, so that the path is straight again.
        obstacleEntity->m_transform.pos = {-10, 0, 0};
        awareness.updateEntityMovement(*avatarEntity, *obstacleEntity);
        rebuildAllTilesFn();

        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(WFMath::Point<2>(20, 0), to2D(steering.getPath()[0]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Move it back again
        obstacleEntity->m_transform.pos = {10, 0, 0};
        awareness.updateEntityMovement(*avatarEntity, *obstacleEntity);
        rebuildAllTilesFn();

        result = steering.updatePath(0);
        ASSERT_EQUAL(3, result); //Should be three verts since we need to divert twice
        ASSERT_EQUAL(WFMath::Point<2>(20, 0), to2D(steering.getPath()[2]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Remove obstacle entity from awareness
        awareness.removeEntity(*avatarEntity, *obstacleEntity);
        rebuildAllTilesFn();
        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line
        ASSERT_EQUAL(WFMath::Point<2>(20, 0), to2D(steering.getPath()[0]))
        ASSERT_FALSE(steering.isAtCurrentDestination(0))

        //Add the small obstacle entity between the avatar and the destination.
        // Since it can be stepped over/on it should not affect the path

        smallObstacleEntity->m_transform.pos = {10, 0, 0};
        awareness.addEntity(*avatarEntity, *smallObstacleEntity, false);
        steering.setDestination({{EntityLocation(worldEntity, {20, 0, 0})}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER, desiredDistance}, 0);
        rebuildAllTilesFn();
        result = steering.updatePath(0);
        ASSERT_EQUAL(1, result); //Should be one vert since it's a straight line


    }

};

int main()
{
    SteeringIntegration t;

    return t.run();
}
