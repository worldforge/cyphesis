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


#include "../TestBaseWithContext.h"
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include "tools/EntityExporterBase.h"


using Atlas::Message::MapType;
using Atlas::Message::ListType;

struct TestContext
{

};


struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{
    Tested()
    {
        ADD_TEST(test_extract_list);
        ADD_TEST(test_extract_map);
    }

    void test_extract_list(TestContext& context)
    {
        {
            ListType typeList{"one", "two", "three"};
            ListType entityList{"one", "two", "three"};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }

        {
            ListType typeList{};
            ListType entityList{};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }

        {
            ListType typeList{"one", "two", "three"};
            ListType entityList{};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }

        {
            ListType typeList{};
            ListType entityList{"one", "two", "three"};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }


        {
            ListType typeList{"one", "two", "three"};
            ListType entityList{"zero", "one", "two", "three", "four"};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{"zero"}, result.prepend)
            ASSERT_EQUAL(ListType{"four"}, result.append)
        }
        {
            ListType typeList{"one", "two", "three"};
            ListType entityList{"zero", "one", "two", "four"};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }
        {
            ListType typeList{"one", "two", "three"};
            ListType entityList{"one", "two"};
            auto result = EntityExporterBase::extractListPrependAppend(typeList, entityList);
            ASSERT_EQUAL(ListType{}, result.prepend)
            ASSERT_EQUAL(ListType{}, result.append)
        }
    }

    void test_extract_map(TestContext& context)
    {
        {
            MapType typeMap{{"one",   1},
                            {"two",   2},
                            {"three", 3}};
            MapType entityMap{{"one",   1},
                              {"two",   2},
                              {"three", 3}};
            auto result = EntityExporterBase::extractMapPrepend(typeMap, entityMap);
            ASSERT_EQUAL(MapType{}, result)
        }

        {
            MapType typeMap{{"one",   1},
                            {"two",   2},
                            {"three", 3}};
            MapType entityMap{{"one",   1},
                              {"two",   2},
                              {"three", 3},
                              {"four",  4}};
            auto result = EntityExporterBase::extractMapPrepend(typeMap, entityMap);
            MapType expected{{"four", 4}};
            ASSERT_EQUAL(expected, result)
        }
        {
            MapType typeMap{{"one",   1},
                            {"two",   2},
                            {"three", 3}};
            MapType entityMap{{"one",  1},
                              {"two",  2},
                              {"four", 4}};
            auto result = EntityExporterBase::extractMapPrepend(typeMap, entityMap);
            MapType expected{};
            ASSERT_EQUAL(expected, result)
        }
    }
};


int main()
{
    Tested t;

    return t.run();
}
