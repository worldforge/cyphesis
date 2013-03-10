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

#include "common/Property.h"
#include "common/PropertyFactory_impl.h"
#include "common/ScriptPropertyFactory_impl.h"

#include <cassert>

class ScriptPropertyFactorytest : public Cyphesis::TestBase
{
  public:
    ScriptPropertyFactorytest();

    void setup();
    void teardown();

    template <class PropertyT>
    void test_factory();
};

ScriptPropertyFactorytest::ScriptPropertyFactorytest()
{
    ADD_TEST(ScriptPropertyFactorytest::test_factory<Property<int>>);
    ADD_TEST(ScriptPropertyFactorytest::test_factory<Property<long>>);
    ADD_TEST(ScriptPropertyFactorytest::test_factory<Property<double>>);
    ADD_TEST(ScriptPropertyFactorytest::test_factory<Property<std::string>>);
}

void ScriptPropertyFactorytest::setup()
{
}

void ScriptPropertyFactorytest::teardown()
{
}

template <class PropertyT>
void ScriptPropertyFactorytest::test_factory()
{
    ScriptPropertyFactory<PropertyFactory<PropertyT>> pf;

    PropertyBase * p = pf.newProperty();

    ASSERT_NOT_NULL(p);
    ASSERT_NOT_NULL(dynamic_cast<PropertyT *>(p));

    PropertyKit * pk = pf.duplicateFactory();

    ASSERT_NOT_NULL(pk);
    ASSERT_NOT_NULL(dynamic_cast<ScriptPropertyFactory<PropertyFactory<PropertyT>> *>(pk));

    pk = pf.scriptPropertyFactory();

    ASSERT_NOT_NULL(pk);
    ASSERT_NOT_NULL(dynamic_cast<ScriptPropertyFactory<PropertyFactory<PropertyT>> *>(pk));
}

int main()
{
    ScriptPropertyFactorytest t;

    return t.run();
}

// stubs

PropertyKit::~PropertyKit()
{
}
