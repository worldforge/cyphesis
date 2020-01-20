// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

class composetest : public Cyphesis::TestBase
{
  public:
    composetest();

    void setup();
    void teardown();

    void test_integer();
    void test_charstar();
    void test_string();
    void test_empty_charstar();
    void test_empty_string();
};

composetest::composetest()
{
    ADD_TEST(composetest::test_integer);
    ADD_TEST(composetest::test_charstar);
    ADD_TEST(composetest::test_string);
    ADD_TEST(composetest::test_empty_charstar);
    ADD_TEST(composetest::test_empty_string);
}

void composetest::setup()
{
}

void composetest::teardown()
{
}

void composetest::test_integer()
{
    ASSERT_EQUAL(String::compose("%1", 42),
                 std::string("42"));
}

void composetest::test_charstar()
{
    const char * test_string = "53868979-193d-4ba6-9737-747c1e07934f";

    ASSERT_EQUAL(String::compose("%1", test_string),
                 std::string(test_string));
}

void composetest::test_string()
{
    const char * test_string = "e68f1131-472f-43e9-90c9-1d2080b17316";

    ASSERT_EQUAL(String::compose("%1", std::string(test_string)),
                 std::string(test_string));
}

void composetest::test_empty_charstar()
{
    ASSERT_EQUAL(String::compose("%1 %2 %3", "", 23, 42),
                 std::string(" 23 42"));
}

void composetest::test_empty_string()
{
    ASSERT_EQUAL(String::compose("%1 %2 %3", std::string(""), 23, 42),
                 std::string(" 23 42"));
}

int main()
{
    composetest t;

    return t.run();
}
