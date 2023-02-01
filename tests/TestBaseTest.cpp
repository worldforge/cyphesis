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

//Note that this test depends on the line count staying the same. If you add or remove a line here in the beginning, make sure to update the line numbers in many of tests.
#include "TestBase.h"

#include "common/compose.hpp"

#include <iostream>

#include <cassert>

using String::compose;

class Test : public Cyphesis::TestBase {
  public:
    void setup()
    {
    }

    void teardown()
    {
    }
};
namespace {
    void test_assertTrue() {
        Test t;
        assert(t.errorCount() == 0);
        t.assertTrue("test_var", true, "test_func", "test_file", 17467);
        assert(t.errorCount() == 0);
        t.assertTrue("test_var", false, "test_func", "test_file", 17467);
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               "test_file:17467: test_func: Assertion 'test_var' failed.");
    }

    void test_assertEqual() {
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

    void test_ASSERT_TRUE() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                bool val = false;
                ASSERT_TRUE(val);
                ASSERT_TRUE(val);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:86: void {anonymous}::test_ASSERT_TRUE()::T::test_macro(): "
                       "Assertion 'val' failed.", __FILE__));
    }

    void test_ASSERT_EQUAL() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int i = 1, j = 2;
                ASSERT_EQUAL(i, j);
                ASSERT_EQUAL(i, j);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:111: void {anonymous}::test_ASSERT_EQUAL()::T::test_macro(): "
                       "Assertion 'i == j' failed. 1 != 2", __FILE__));
    }

    void test_ASSERT_NOT_EQUAL() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int i = 1, j = 1;
                ASSERT_NOT_EQUAL(i, j);
                ASSERT_NOT_EQUAL(i, j);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:136: void {anonymous}::test_ASSERT_NOT_EQUAL()::T::test_macro(): "
                       "Assertion 'i != j' failed. 1 == 1", __FILE__));
    }

    void test_ASSERT_GREATER() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int i = 1, j = 2;
                ASSERT_GREATER(i, j);
                ASSERT_GREATER(i, j);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:161: void {anonymous}::test_ASSERT_GREATER()::T::test_macro(): "
                       "Assertion 'i > j' failed. 1 <= 2", __FILE__));
    }

    void test_ASSERT_LESS() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int i = 3, j = 2;
                ASSERT_LESS(i, j);
                ASSERT_LESS(i, j);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:186: void {anonymous}::test_ASSERT_LESS()::T::test_macro(): "
                       "Assertion 'i < j' failed. 3 >= 2", __FILE__));
    }

    void test_ASSERT_NULL() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int j;
                int *i = &j;
                ASSERT_NULL(&i);
                ASSERT_NULL(&i);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:212: void {anonymous}::test_ASSERT_NULL()::T::test_macro(): "
                       "Assertion '&i' null failed.", __FILE__));
    }

    void test_ASSERT_NOT_NULL() {
        class T : public Cyphesis::TestBase {
        public:
            void setup() {
            }

            void teardown() {
            }

            void test_macro() {
                int *i = 0;
                ASSERT_NOT_NULL(i);
                ASSERT_NOT_NULL(i);
            }
        } t;

        assert(t.errorCount() == 0);
        t.test_macro();
        assert(t.errorCount() == 1);

        assert(t.errorReports().front() ==
               compose("%1:237: void {anonymous}::test_ASSERT_NOT_NULL()::T::test_macro(): "
                       "Assertion 'i' not null failed.", __FILE__));
    }
}
int main()
{
    test_assertTrue();
    test_assertEqual();
    test_ASSERT_TRUE();
    test_ASSERT_EQUAL();
    test_ASSERT_NOT_EQUAL();
    test_ASSERT_GREATER();
    test_ASSERT_LESS();
    test_ASSERT_NULL();
    test_ASSERT_NOT_NULL();
    return 0;
}
