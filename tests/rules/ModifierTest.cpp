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
#include "rules/Modifier.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
struct TestContext {
};

struct Tested : public Cyphesis::TestBaseWithContext<TestContext> {
    Tested()
    {
        ADD_TEST(test_modifiers_add_fraction);
        ADD_TEST(test_modifiers_subtract);
        ADD_TEST(test_modifiers_prepend);
        ADD_TEST(test_modifiers_append);
        ADD_TEST(test_modifiers_default);
    }
    void test_modifiers_add_fraction(const TestContext& context)
    {
        Element e;
        {
            AddFractionModifier mod(2);
            e = 30;
            mod.process(e, e);
            ASSERT_EQUAL(90, e.asInt())

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(6.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(), e)
        }

        {
            auto the_value = 2.0f;
            AddFractionModifier mod(the_value);
            e = 30;
            mod.process(e, e);
            ASSERT_EQUAL(Element(90), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(6.0, e.asFloat())

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(), e)
        }

        {
            auto the_value = "a string";
            AddFractionModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "string";
            mod.process(e, e);
            ASSERT_EQUAL(Element("string"), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{{"foo", "bar"}};
            mod.process(e, e);
            auto result = MapType{{"foo", "bar"}};
            ASSERT_EQUAL(Element(result), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(), e)
        }

        {
            auto the_value = MapType{{"foo", "bar"}};
            AddFractionModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{};
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(), e)
        }

        {
            auto the_value = ListType{"bar"};
            AddFractionModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(), e)
        }

        {
            auto the_value = Element();
            AddFractionModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())
        }
    }

    void test_modifiers_subtract(const TestContext& context)
    {
        Element e;
        {
            auto the_value = 1;
            SubtractModifier mod(the_value);
            e = 3;
            mod.process(e, e);
            ASSERT_EQUAL(2, e.asInt())

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(-the_value), e)
        }

        {
            auto the_value = 1.0f;
            SubtractModifier mod(the_value);
            e = 3;
            mod.process(e, e);
            ASSERT_EQUAL(Element(3), e)

            e = 3.0f;
            mod.process(e, e);
            ASSERT_EQUAL(2.0, e.asFloat())

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(-the_value, e.asFloat())
        }

        {
            auto the_value = "a string";
            SubtractModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "string";
            mod.process(e, e);
            ASSERT_EQUAL(Element("string"), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{{"foo", "bar"}};
            mod.process(e, e);
            auto result = MapType{{"foo", "bar"}};
            ASSERT_EQUAL(Element(result), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)
        }

        {
            auto the_value = MapType{{"foo", "bar"}};
            SubtractModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{{"one", "two"}, {"foo", "bar"}};
            mod.process(e, e);
            MapType result{
                {"one", "two"},
            };
            ASSERT_EQUAL(result, e.asMap())

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(MapType(), e.asMap())
        }

        {
            auto the_value = ListType{"bar"};
            SubtractModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType{"foo", "bar"};
            mod.process(e, e);
            ListType result{"foo"};
            ASSERT_EQUAL(result, e.asList())

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)
        }

        {
            auto the_value = Element();
            SubtractModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())
        }
    }

    void test_modifiers_prepend(const TestContext& context)
    {
        Element e;
        {
            auto the_value = 1;
            PrependModifier mod(the_value);
            e = 1;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = 1.0f;
            PrependModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(3.0, e.asFloat())

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = "a string";
            PrependModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "foo";
            mod.process(e, e);
            ASSERT_EQUAL("a stringfoo", e.asString())

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = MapType{{"foo", "bar"}};
            PrependModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{{"one", "two"}};
            mod.process(e, e);
            MapType result{{"one", "two"}, {"foo", "bar"}};
            ASSERT_EQUAL(result, e.asMap())

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = ListType{"foo", "bar"};
            PrependModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType{"single"};
            mod.process(e, e);
            ListType result{"foo", "bar", "single"};
            ASSERT_EQUAL(result, e.asList())

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = Element();
            PrependModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())
        }
    }

    void test_modifiers_append(const TestContext& context)
    {
        Element e;
        {
            auto the_value = 1;
            AppendModifier mod(the_value);
            e = 1;
            mod.process(e, e);
            ASSERT_EQUAL(2, e.asInt())

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = 1.0f;
            AppendModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(3.0, e.asFloat())

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = "a string";
            AppendModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "foo";
            mod.process(e, e);
            ASSERT_EQUAL("fooa string", e.asString())

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = MapType{{"foo", "bar"}};
            AppendModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType{{"one", "two"}};
            mod.process(e, e);
            MapType result{{"one", "two"}, {"foo", "bar"}};
            ASSERT_EQUAL(result, e.asMap())

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = ListType{"foo", "bar"};
            AppendModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType{"single"};
            mod.process(e, e);
            ListType result{"single", "foo", "bar"};
            ASSERT_EQUAL(result, e.asList())

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = Element();
            AppendModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(0), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(2.0f), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(""), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(ListType()), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(MapType()), e)

            e = Element();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())
        }
    }

    void test_modifiers_default(const TestContext& context)
    {
        Element e;
        {
            auto the_value = 1;
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = 1.0f;
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = "a string";
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = MapType{{"foo", "bar"}};
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = ListType{"foo", "bar"};
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = 2.0f;
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = "";
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = ListType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = MapType();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)

            e = Element();
            mod.process(e, e);
            ASSERT_EQUAL(Element(the_value), e)
        }

        {
            auto the_value = Element();
            DefaultModifier mod(the_value);
            e = 0;
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())

            e = 2.0f;
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())

            e = "";
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())

            e = ListType();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())

            e = MapType();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())

            e = Element();
            mod.process(e, e);
            ASSERT_TRUE(the_value.isNone())
        }
    }
};

int main()
{
    Tested t;

    return t.run();
}
