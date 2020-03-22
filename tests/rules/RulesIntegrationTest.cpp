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

#include "../TestBase.h"
#include "../DatabaseNull.h"
#include "../TestWorld.h"
#include "common/Monitors.h"
#include "common/Inheritance.h"
#include "rules/simulation/World.h"

Atlas::Objects::Factories factories;

struct Tested : public Cyphesis::TestBase
{

    struct TestContext
    {
        DatabaseNull database;
        Ref<World> world;
        Inheritance inheritance;
        TestWorld testWorld;

        TestContext() :
                world(new World()),
                inheritance(factories),
                testWorld(world)
        {


        }

    };


    Tested()
    {
        ADD_TEST(Tested::test_setProps);
    }

    void setup() override
    {

    }

    void teardown() override
    {

    }

    void test_setProps()
    {
        TestContext context;

        Ref<Entity> entity(new Entity("1", 1));



    }


};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}
