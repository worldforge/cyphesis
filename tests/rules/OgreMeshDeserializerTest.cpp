/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"
#include "rules/simulation/OgreMeshDeserializer.h"

class OgreMeshDeserializerTest : public Cyphesis::TestBase
{
    private:

    public:
        OgreMeshDeserializerTest();

        void setup() override;

        void teardown() override;

        void test_readBboxMesh();


};

OgreMeshDeserializerTest::OgreMeshDeserializerTest()
{
    ADD_TEST(OgreMeshDeserializerTest::test_readBboxMesh);
}

void OgreMeshDeserializerTest::test_readBboxMesh()
{
    std::ifstream fileStream(TESTDATADIR "/box.mesh");

    ASSERT_TRUE(fileStream);

    OgreMeshDeserializer deserializer(fileStream);

    deserializer.deserialize();

    ASSERT_EQUAL(deserializer.m_vertices.size(), 24u * 3u);
    ASSERT_EQUAL(deserializer.m_vertices[0], 1);
    ASSERT_EQUAL(deserializer.m_vertices[1], -1);
    ASSERT_EQUAL(deserializer.m_vertices[2], -1);
    ASSERT_EQUAL(deserializer.m_vertices[69], -1);
    ASSERT_EQUAL(deserializer.m_vertices[70], 1);
    ASSERT_EQUAL(deserializer.m_vertices[71], -1);

    ASSERT_EQUAL(deserializer.m_indices.size(), 12u * 3u);
    ASSERT_EQUAL(deserializer.m_indices[0], 0u);
    ASSERT_EQUAL(deserializer.m_indices[1], 1u);
    ASSERT_EQUAL(deserializer.m_indices[2], 2u);
    ASSERT_EQUAL(deserializer.m_indices[33], 22u);
    ASSERT_EQUAL(deserializer.m_indices[34], 23u);
    ASSERT_EQUAL(deserializer.m_indices[35], 20u);


    ASSERT_TRUE(deserializer.m_bounds.isValid());
    ASSERT_EQUAL(deserializer.m_bounds.lowCorner().x(), -1.0);
    ASSERT_EQUAL(deserializer.m_bounds.lowCorner().y(), -1.0);
    ASSERT_EQUAL(deserializer.m_bounds.lowCorner().z(), -1.0);
    ASSERT_EQUAL(deserializer.m_bounds.highCorner().x(), 1.0);
    ASSERT_EQUAL(deserializer.m_bounds.highCorner().y(), 1.0);
    ASSERT_EQUAL(deserializer.m_bounds.highCorner().z(), 1.0);
    ASSERT_FUZZY_EQUAL(deserializer.m_radius, 1.73205, 0.0001);

}

void OgreMeshDeserializerTest::setup()
{
}

void OgreMeshDeserializerTest::teardown()
{
}

int main()
{
    OgreMeshDeserializerTest t;

    return t.run();
}
