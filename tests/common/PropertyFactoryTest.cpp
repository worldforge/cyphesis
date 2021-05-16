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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"

#include "common/PropertyFactory_impl.h"
#include "common/Property.h"
#include "common/Property_impl.h"

#include <cassert>

class PropertyFactorytest : public Cyphesis::TestBase
{
  public:
    PropertyFactorytest();

    void setup();
    void teardown();

    template <class PropertyT>
    void test_factory();
};

PropertyFactorytest::PropertyFactorytest()
{
    ADD_TEST(PropertyFactorytest::test_factory<Property<int>>);
    ADD_TEST(PropertyFactorytest::test_factory<Property<long>>);
    ADD_TEST(PropertyFactorytest::test_factory<Property<double>>);
    ADD_TEST(PropertyFactorytest::test_factory<Property<std::string>>);
}

void PropertyFactorytest::setup()
{
}

void PropertyFactorytest::teardown()
{
}

template <class PropertyT>
void PropertyFactorytest::test_factory()
{
    PropertyFactory<PropertyT> pf;

    auto p = pf.newProperty();

    ASSERT_TRUE(p);
    ASSERT_NOT_NULL(dynamic_cast<PropertyT *>(p.get()));

    auto pk = pf.duplicateFactory();

    ASSERT_NOT_NULL(pk.get());
    ASSERT_NOT_NULL(dynamic_cast<PropertyFactory<PropertyT> *>(pk.get()));
}

int main()
{
    PropertyFactorytest t;

    return t.run();
}

#include "../stubs/common/stublog.h"
