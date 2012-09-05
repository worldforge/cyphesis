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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include <iostream>

#include <cassert>

class Test : public Cyphesis::TestBase {
  public:
    void setup()
    {
    }

    void teardown()
    {
    }
};

void test_assertTrue()
{
    Test t;
    assert(t.errorCount() == 0);
    t.assertTrue("test_var", true, "test_func", "test_file", 17467);
    assert(t.errorCount() == 0);
    t.assertTrue("test_var", false, "test_func", "test_file", 17467);
    assert(t.errorCount() == 1);

    assert(t.errorReports().front() ==
           "test_file:17467: test_func: Assertion 'test_var' failed.");
}

void test_assertEqual()
{
    Test t;
    assert(t.errorCount() == 0);
    t.assertEqual("test_lval", 1, "test_rval", 1, "test_func",
                  "test_file", 96236);
    assert(t.errorCount() == 0);
    t.assertEqual("test_lval", 1, "test_rval", 2, "test_func",
                  "test_file", 96236);
    assert(t.errorCount() == 1);

    assert(t.errorReports().front() ==
           "test_file:96236: test_func: Assertion 'test_lval == test_rval' "
           "failed. 1 != 2");
}

void test_ASSERT_TRUE()
{
    class T : public Cyphesis::TestBase {
      public:
        void setup()
        {
        }

        void teardown()
        {
        }
        
        void test_macro()
        {
            ASSERT_TRUE(false);
            ASSERT_TRUE(false);
        }
    } t;

    assert(t.errorCount() == 0);
    t.test_macro();
    assert(t.errorCount() == 1);

    assert(t.errorReports().front() ==
           "TestBasetest.cpp:87: void test_ASSERT_TRUE()::T::test_macro(): "
           "Assertion 'false' failed.");
}

void test_ASSERT_EQUAL()
{
    class T : public Cyphesis::TestBase {
      public:
        void setup()
        {
        }

        void teardown()
        {
        }
        
        void test_macro()
        {
            ASSERT_EQUAL(1, 2);
            ASSERT_EQUAL(1, 2);
        }
    } t;

    assert(t.errorCount() == 0);
    t.test_macro();
    assert(t.errorCount() == 1);

    assert(t.errorReports().front() ==
           "TestBasetest.cpp:115: void test_ASSERT_EQUAL()::T::test_macro(): "
           "Assertion '1 == 2' failed. 1 != 2");
}

void test_ASSERT_NULL()
{
    class T : public Cyphesis::TestBase {
      public:
        void setup()
        {
        }

        void teardown()
        {
        }
        
        void test_macro()
        {
            int i;
            ASSERT_NULL(&i);
            ASSERT_NULL(&i);
        }
    } t;

    assert(t.errorCount() == 0);
    t.test_macro();
    assert(t.errorCount() == 1);

    assert(t.errorReports().front() ==
           "TestBasetest.cpp:144: void test_ASSERT_NULL()::T::test_macro(): "
           "Assertion '&i' null failed.");
}

void test_ASSERT_NOT_NULL()
{
    class T : public Cyphesis::TestBase {
      public:
        void setup()
        {
        }

        void teardown()
        {
        }
        
        void test_macro()
        {
            int * i = 0;
            ASSERT_NOT_NULL(i);
            ASSERT_NOT_NULL(i);
        }
    } t;

    assert(t.errorCount() == 0);
    t.test_macro();
    assert(t.errorCount() == 1);

    std::cout << t.errorReports().front() << std::endl;
    assert(t.errorReports().front() ==
           "TestBasetest.cpp:173: void test_ASSERT_NOT_NULL()::T::test_macro(): "
           "Assertion 'i' not null failed.");
}

int main()
{
    test_assertTrue();
    test_assertEqual();
    test_ASSERT_TRUE();
    test_ASSERT_EQUAL();
    test_ASSERT_NULL();
    test_ASSERT_NOT_NULL();
    return 0;
}
