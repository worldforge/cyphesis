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
#include "rules/MemEntity.h"
#include "../TestBase.h"
#include "../TestWorld.h"

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


struct SteeringIntegration : public Cyphesis::TestBase
{
    static long m_id_counter;

    SteeringIntegration()
    {
        ADD_TEST(SteeringIntegration::test_create);
        ADD_TEST(SteeringIntegration::test_resolveDestination);
        ADD_TEST(SteeringIntegration::test_distance);
    }

    void setup()
    {

    }

    void teardown()
    {

    }

    void test_create()
    {
        Ref<MemEntity> avatarEntity(new MemEntity("1", 1));
        avatarEntity->m_location.m_bBox = {{-1, -1, -1},
                                           {1,  1,  1}};
        Steering steering(*avatarEntity);
    }

    void test_resolveDestination()
    {
        Ref<MemEntity> worldEntity(new MemEntity("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntity("1", 1));
        avatarEntity->m_location.m_pos = {0, 0, 0};
        avatarEntity->m_location.m_bBox = {{-1, 0, -1},
                                           {1,  1, 1}};
        Ref<MemEntity> otherEntity(new MemEntity("2", 2));
        otherEntity->m_location.m_pos = {10, 0, 0};
        Ref<MemEntity> outOfWorldEntity(new MemEntity("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntity("4", 4));
        Ref<MemEntity> otherChildEntity(new MemEntity("5", 5));

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        avatarEntity->addChild(*avatarChildEntity);
        otherEntity->addChild(*otherChildEntity);
        Steering steering(*avatarEntity);

        ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.resolvePosition(0, {otherEntity->m_location}).position);
        ASSERT_EQUAL(otherEntity->m_location, *steering.resolvePosition(0, {otherEntity->m_location}).location);
        ASSERT_EQUAL(WFMath::Point<3>(10, 0, 0), steering.resolvePosition(0, {otherChildEntity->m_location}).position);
        ASSERT_EQUAL(otherEntity->m_location, *steering.resolvePosition(0, {otherChildEntity->m_location}).location);
        ASSERT_EQUAL(WFMath::Point<3>(0, 0, 0), steering.resolvePosition(0, {avatarChildEntity->m_location}).position);
        ASSERT_EQUAL(avatarEntity->m_location, *steering.resolvePosition(0, {avatarChildEntity->m_location}).location);
    }

    void test_distance()
    {
        Ref<MemEntity> worldEntity(new MemEntity("0", 0));
        Ref<MemEntity> avatarEntity(new MemEntity("1", 1));
        avatarEntity->m_location.m_pos = {0, 0, 0};
        avatarEntity->m_location.m_bBox = {{-1, 0, -1},
                                           {1,  1, 1}};
        auto avatarHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(avatarEntity->m_location.m_bBox));
        ASSERT_FUZZY_EQUAL(1.41421, avatarHorizontalRadius, epsilon);
        Ref<MemEntity> otherEntity(new MemEntity("2", 2));
        otherEntity->m_location.m_pos = {10, 0, 0};
        otherEntity->m_location.m_bBox = {{-2, 0, -2},
                                          {2,  3, 2}};
        auto otherHorizontalRadius = std::sqrt(boxSquareHorizontalBoundingRadius(otherEntity->m_location.m_bBox));
        ASSERT_FUZZY_EQUAL(2.828425, otherHorizontalRadius, epsilon);
        Ref<MemEntity> outOfWorldEntity(new MemEntity("3", 3));
        Ref<MemEntity> avatarChildEntity(new MemEntity("4", 4));

        worldEntity->addChild(*avatarEntity);
        worldEntity->addChild(*otherEntity);
        avatarEntity->addChild(*avatarChildEntity);
        Steering steering(*avatarEntity);

        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(10.0 - otherHorizontalRadius, *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::EDGE), epsilon);
        ASSERT_FUZZY_EQUAL(10.0 - otherHorizontalRadius - avatarHorizontalRadius,
                           *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);
        //Move other away
        otherEntity->m_location.m_pos = {20, 0, 0};
        ASSERT_FUZZY_EQUAL(20.0, *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(20.0 - otherHorizontalRadius, *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::EDGE), epsilon);
        ASSERT_FUZZY_EQUAL(20.0 - otherHorizontalRadius - avatarHorizontalRadius,
                           *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);
        //Move avatar closer
        avatarEntity->m_location.m_pos = {10, 0, 0};
        ASSERT_FUZZY_EQUAL(10.0, *steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);

        //Test invalid positions
        ASSERT_FALSE(steering.distanceTo(0, Steering::Destination{outOfWorldEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER))
        ASSERT_FALSE(steering.distanceTo(0, Steering::Destination{outOfWorldEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::CENTER))
        ASSERT_FALSE(steering.distanceTo(0, Steering::Destination{outOfWorldEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE))

        otherEntity->m_location.m_pos = {};
        ASSERT_FALSE(steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        otherEntity->m_location.m_pos = {20, 0, 0};
        avatarEntity->m_location.m_pos = {};
        ASSERT_FALSE(steering.distanceTo(0, Steering::Destination{otherEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        avatarEntity->m_location.m_pos = {10, 0, 0};

        //The distance to the avatar child entity should be 0.
        ASSERT_TRUE(steering.distanceTo(0, Steering::Destination{avatarChildEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER));
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, Steering::Destination{avatarChildEntity->m_location}, Steering::MeasureType::CENTER, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, Steering::Destination{avatarChildEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::CENTER), epsilon);
        ASSERT_FUZZY_EQUAL(0, *steering.distanceTo(0, Steering::Destination{avatarChildEntity->m_location}, Steering::MeasureType::EDGE, Steering::MeasureType::EDGE), epsilon);

    }
};

int main()
{
    SteeringIntegration t;

    return t.run();
}
