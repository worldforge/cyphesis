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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"

#include "rules/Location.h"
#include "common/log.h"

#include "server/SpawnEntity.h"

#include "rules/simulation/AreaProperty.h"

#include "physics/Vector3D.h"


#include <Atlas/Message/Element.h>

#include <wfmath/point.h>

#include <cassert>

class SpawnEntitytest : public Cyphesis::TestBase
{
  private:
    SpawnEntity * m_spawn;
  public:
    SpawnEntitytest();

    void setup();
    void teardown();

    void test_constructor();
    void test_setup_string();
    void test_setup_list();
    void test_setup_contains_string();
    void test_setup_contains_list();
};

SpawnEntitytest::SpawnEntitytest()
{
    ADD_TEST(SpawnEntitytest::test_constructor);
    ADD_TEST(SpawnEntitytest::test_setup_string);
    ADD_TEST(SpawnEntitytest::test_setup_list);
    ADD_TEST(SpawnEntitytest::test_setup_contains_string);
    ADD_TEST(SpawnEntitytest::test_setup_contains_list);
}

void SpawnEntitytest::setup()
{
    m_spawn = new SpawnEntity(0);
}

void SpawnEntitytest::teardown()
{
    delete m_spawn;
}

void SpawnEntitytest::test_constructor()
{
    Spawn * s = new SpawnEntity(0);

    delete s;
}

void SpawnEntitytest::test_setup_string()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["character_types"] = "bob";
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_list()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["character_types"] = Atlas::Message::ListType(1, "bob");
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_contains_string()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["contains"] = "bob";
    m_spawn->setup(spawn_data);
}

void SpawnEntitytest::test_setup_contains_list()
{
    Atlas::Message::MapType spawn_data;
    spawn_data["contains"] = Atlas::Message::ListType(1, "bob");
    m_spawn->setup(spawn_data);
}

int main()
{
    SpawnEntitytest t;

    return t.run();
}

// Stubs

#include "../stubs/common/stubProperty.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}
#include "../stubs/modules/stubWeakEntityRef.h"
#include "../stubs/rules/simulation/stubAreaProperty.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"

void log(LogLevel, const std::string & msg)
{

}
