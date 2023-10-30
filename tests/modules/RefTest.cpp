/*
 Copyright (C) 2018 Erik Ogenvik

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

#include "modules/Ref.h"
#include "../TestBase.h"

#include <set>

struct RefTest : public Cyphesis::TestBase
{
    struct RefCounted
    {
        long& count;

        RefCounted(long& count_ref)
                : count(count_ref)
        {

        }

        void incRef()
        {
            count++;
        }

        virtual int decRef()
        {
            count--;
            return count;
        }
    };

    struct RefCountedChild : public RefCounted
    {
        RefCountedChild(long& count_ref) : RefCounted(count_ref)
        {}

    };

    struct RefCountedDeleteMarker : public RefCounted
    {
        bool& delete_marker;

        RefCountedDeleteMarker(long& count_ref, bool& delete_marker_)
                : RefCounted(count_ref), delete_marker(delete_marker_)
        {

        }

        ~RefCountedDeleteMarker() {
            delete_marker = true;
        }

    };

    void setup()
    {
    }

    void teardown()
    {
    }

    void test_refcount()
    {
        long count1 = 1;
        auto r1 = new RefCounted(count1);
        {
            Ref<RefCounted> t0{};
            ASSERT_NULL(t0.get());

            Ref<RefCounted> t1(r1);
            ASSERT_EQUAL(2, count1);
            {

                auto t1_1 = t1;
                ASSERT_EQUAL(3, count1);
            }
            ASSERT_EQUAL(2, count1);
            t1 = nullptr;
            ASSERT_EQUAL(1, count1);
            t1 = nullptr;
            ASSERT_EQUAL(1, count1);
            t1 = r1;
            ASSERT_EQUAL(2, count1);
            t1 = r1;
            ASSERT_EQUAL(2, count1);
            ASSERT_EQUAL(r1, t1.get());

            auto t2 = std::move(t1);
            ASSERT_EQUAL(2, count1);
            ASSERT_NULL(t1.get());

            ASSERT_FALSE(t1 == t2);
            ASSERT_FALSE(t1.get() == r1);
            ASSERT_TRUE(t2.get() == r1);
            Ref<RefCounted> t3(r1);
            ASSERT_EQUAL(3, count1);
            ASSERT_TRUE(t2 == t3);
            ASSERT_EQUAL(3, t2->count)
            ASSERT_EQUAL(3, (*t2).count)
        }
        delete r1;
    }


    void test_conversion()
    {
        long count1 = 1;
        auto r1 = new RefCountedChild(count1);
        {
            Ref<RefCountedChild> t1(r1);
            ASSERT_EQUAL(2, count1);

            Ref<RefCounted> parent1 = r1;
            ASSERT_EQUAL(3, count1);

            Ref<RefCounted> parent2 = t1;
            ASSERT_EQUAL(4, count1);

            Ref<RefCounted> parent3;
            parent3 = t1;

            ASSERT_TRUE(parent3 == parent2);
            ASSERT_TRUE(parent3 == t1);
            ASSERT_FALSE(parent3 != parent2);
            ASSERT_FALSE(parent3 != t1);
        }
        delete r1;
    }

    void test_container()
    {
        long count1 = 1;
        auto r1 = new RefCounted(count1);
        {
            std::set<Ref<RefCounted>> set;
            set.insert(r1);
            ASSERT_EQUAL(1u, set.size());
            set.insert(r1);
            ASSERT_EQUAL(1u, set.size());
            set.insert(Ref<RefCounted>(r1));
            ASSERT_EQUAL(1u, set.size());
            set.erase(r1);
            ASSERT_TRUE(set.empty());
        }
        delete r1;
    }

    void test_deletion()
    {
        {
            long count1 = 0;
            bool delete_marker1 = false;
            auto m1 = new RefCountedDeleteMarker(count1, delete_marker1);
            {
                Ref<RefCountedDeleteMarker> r1(m1);
                ASSERT_FALSE(delete_marker1);
            }
            ASSERT_TRUE(delete_marker1);
            ASSERT_EQUAL(0, count1);
        }

        {
            long count1 = 0;
            bool delete_marker1 = false;
            auto m1 = new RefCountedDeleteMarker(count1, delete_marker1);
            {
                Ref<RefCountedDeleteMarker> r1(m1);
                ASSERT_FALSE(delete_marker1);
                r1 = r1;
                ASSERT_FALSE(delete_marker1);
                r1 = std::move(r1);
                ASSERT_FALSE(delete_marker1);
            }
            ASSERT_TRUE(delete_marker1);
            ASSERT_EQUAL(0, count1);
        }
        {
            long count1 = 0;
            bool delete_marker1 = false;
            auto m1 = new RefCountedDeleteMarker(count1, delete_marker1);
            {
                Ref<RefCountedDeleteMarker> r1(m1);
                ASSERT_FALSE(delete_marker1);
                Ref<RefCounted> r2 = r1;
                ASSERT_FALSE(delete_marker1);
                Ref<RefCounted> r3 = std::move(r1);
                ASSERT_FALSE(delete_marker1);
            }
            ASSERT_TRUE(delete_marker1);
            ASSERT_EQUAL(0, count1);
        }

    }

    RefTest()
    {
        ADD_TEST(RefTest::test_refcount);
        ADD_TEST(RefTest::test_conversion);
        ADD_TEST(RefTest::test_container);
        ADD_TEST(RefTest::test_deletion);

    }

};

int main()
{
    return RefTest{}.run();
}
