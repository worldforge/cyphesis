// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "common/PropertyManager.h"

#include <cassert>

class TestPropertyManager : public PropertyManager {
  public:
    TestPropertyManager() { }

    virtual PropertyBase * addProperty(const std::string &, int) {
        return 0;
    }
};

class PropertyManagertest : public Cyphesis::TestBase
{
  private:
    PropertyManager * m_pm;
  public:
    PropertyManagertest();

    void setup();
    void teardown();

    void test_interface();
};

PropertyManagertest::PropertyManagertest()
{
    ADD_TEST(PropertyManagertest::test_interface);
}

void PropertyManagertest::setup()
{
    m_pm = new TestPropertyManager;
}

void PropertyManagertest::teardown()
{
    delete m_pm;
}

void PropertyManagertest::test_interface()
{
    auto * ret = m_pm->addProperty("one", 1);

    ASSERT_NULL(ret);
}

int main()
{
    PropertyManagertest t;

    return t.run();
}
