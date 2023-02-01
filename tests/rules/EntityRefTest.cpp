// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "modules/WeakEntityRef.h"

#include "rules/simulation/Entity.h"

#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"

#include <sigc++/functors/ptr_fun.h>

#include <cassert>

static bool emitted;

static void signal_emitted()
{
    emitted = true;
}
namespace {
    void checkSignal() {
        {
            // Check the assignment operator causes the signal to fire
            emitted = false;

            Entity e(1);
            WeakEntityRef ref;

            assert(emitted == false);

            ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

            assert(emitted == false);

            ref = WeakEntityRef(&e);

            assert(ref.get() == &e);
            assert(emitted == true);
        }

        {
            // Check the assignment operator does not cause the signal to fire
            // the the pointer is unchanged
            emitted = false;

            Entity e(1);
            WeakEntityRef ref(&e);

            assert(emitted == false);

            ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

            assert(emitted == false);

            ref = WeakEntityRef(&e);

            assert(ref.get() == &e);
            assert(emitted == false);
        }

        {
            // Check that destroying the Entity makes the reference null.
            emitted = false;

            Entity e(1);
            Ref<Entity> container = new Entity(2);

            // Set the location of the entity being tested, as destroy requires it.
            e.m_parent = container.get();
            // Make sure the container has a contains structure, as destroy
            // requires it.
            container->m_contains.reset(new LocatedEntitySet);

            WeakEntityRef ref(&e);

            assert(ref);
            assert(emitted == false);

            ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

            assert(ref.get() == &e);
            assert(emitted == false);

            e.destroy();

            assert(ref.get() == 0);
            assert(emitted == true);
            assert(!ref);
        }
    }
}
int main()
{
    {
        // Check the default constructor
        WeakEntityRef ref;
    }

    {
        // Check the default constructor initialises to nullptr via get
        WeakEntityRef ref;

        assert(ref.get() == 0);
    }

    {
        // Check the default constructor initialises to nullptr via dereference
        WeakEntityRef ref;

        assert(&(*ref) == 0);
    }

    {
        // Check the default constructor initialises to nullptr via ->
        WeakEntityRef ref;

        assert(ref.operator->() == 0);
    }

    {
        // Check the default constructor initialises to nullptr via ==
        WeakEntityRef ref;

        assert(ref == 0);
    }

    {
        // Check the initialising constructor via get
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);

        assert(ref.get() == e.get());
    }

    {
        // Check the initialising constructor via dereference
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);

        assert(&(*ref) == e.get());
    }

    {
        // Check the initialising constructor via ->
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);

        assert(ref.operator->() == e.get());
    }

    {
        // Check the initialising constructor via ==
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);

        assert(ref == e.get());
    }

    {
        // Check the copy constructor
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);
        WeakEntityRef ref2(ref);

        assert(ref2.get() == e.get());
    }

    {
        // Check the comparison operator
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);
        WeakEntityRef ref2(e);

        assert(ref == ref2);
    }

    {
        // Check the comparison operator
        Ref<Entity>  e = new Entity(1);
        Ref<Entity>  e2 = new Entity(2);
        WeakEntityRef ref(e);
        WeakEntityRef ref2(e2);

        assert(!(ref == ref2));
    }

#if 0
    // These tests should be included should we add operator!=
    {
        // Check the comparison operator
        Entity e(1);
        WeakEntityRef ref(&e);
        WeakEntityRef ref2(&e);

        assert(!(ref != ref2));
    }

    {
        // Check the comparison operator
        Entity e(1);
        Entity e2(2);
        WeakEntityRef ref(&e);
        WeakEntityRef ref2(&e2);

        assert(ref != ref2);
    }
#endif

    {
        // Check the less than operator
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref(e);
        WeakEntityRef ref2(e);

        assert(!(ref < ref2) && !(ref2 < ref));
    }

    {
        // Check the less than operator
        Ref<Entity>  e = new Entity(1);
        Ref<Entity>  e2 = new Entity(2);
        WeakEntityRef ref(e);
        WeakEntityRef ref2(e2);

        assert(ref < ref2 || ref2 < ref);
    }

    {
        // Check the assignment operator
        Ref<Entity>  e = new Entity(1);
        WeakEntityRef ref;

        ref = WeakEntityRef(e);

        assert(ref.get() == e.get());
    }

    {
        // Check that destroying the Entity makes the reference null.
        Entity e(1);
        Ref<Entity>  container = new Entity(2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_parent = container.get();
        // Make sure the container has a contains structure, as destroy
        // requires it.
        container->m_contains.reset(new LocatedEntitySet);

        WeakEntityRef ref(&e);

        assert(ref.get() == &e);
        e.destroy();
        assert(ref.get() == 0);
    }

    checkSignal();
}
