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

// $Id: EntityReftest.cpp,v 1.3 2008-01-07 01:35:33 alriddoch Exp $

#include "modules/EntityRef.h"

#include "rulesets/Entity.h"

#include <sigc++/functors/ptr_fun.h>

#include <cassert>

static bool emitted;

static void signal_emitted()
{
    emitted = true;
}

void checkSignal()
{
    {
        // Check the assignment operator causes the signal to fire
        emitted = false;

        Entity e("1", 1);
        EntityRef ref;

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(emitted == false);

        ref = EntityRef(&e);

        assert(ref.get() == &e);
        assert(emitted == true);
    }

    {
        // Check the assignment operator does not cause the signal to fire
        // the the pointer is unchanged
        emitted = false;

        Entity e("1", 1);
        EntityRef ref(&e);

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(emitted == false);

        ref = EntityRef(&e);

        assert(ref.get() == &e);
        assert(emitted == false);
    }

    {
        // Check that destroying the Entity makes the reference null.
        emitted = false;

        Entity e("1", 1);
        Entity * container = new Entity("2", 2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_location.m_loc = container;
        // Make sure the container has a contains structure, as destroy
        // requires it.
        container->m_contains = new LocatedEntitySet;
        // Increment the refcount on the container, else the tested Entity's
        // destructor will delete it.
        container->incRef();

        EntityRef ref(&e);

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(ref.get() == &e);
        assert(emitted == false);

        e.destroy();

        assert(ref.get() == 0);
        assert(emitted == true);
    }
}

int main()
{
    {
        // Check the default constructor
        EntityRef ref;
    }

    {
        // Check the default constructor initialises to NULL via get
        EntityRef ref;

        assert(ref.get() == 0);
    }

    {
        // Check the default constructor initialises to NULL via dereference
        EntityRef ref;

        assert(&(*ref) == 0);
    }

    {
        // Check the default constructor initialises to NULL via ->
        EntityRef ref;

        assert(ref.operator->() == 0);
    }

    {
        // Check the default constructor initialises to NULL via ==
        EntityRef ref;

        assert(ref == 0);
    }

    {
        // Check the initialising constructor via get
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref.get() == e);
    }

    {
        // Check the initialising constructor via dereference
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(&(*ref) == e);
    }

    {
        // Check the initialising constructor via ->
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref.operator->() == e);
    }

    {
        // Check the initialising constructor via ==
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref == e);
    }

    {
        // Check the copy constructor
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(ref);

        assert(ref2.get() == e);
    }

    {
        // Check the comparison operator
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(e);

        assert(ref == ref2);
    }

    {
        // Check the comparison operator
        Entity * e = new Entity("1", 1);
        Entity * e2 = new Entity("2", 2);
        EntityRef ref(e);
        EntityRef ref2(e2);

        assert(!(ref == ref2));
    }

#if 0
    // These tests should be included should we add operator!=
    {
        // Check the comparison operator
        Entity e("1", 1);
        EntityRef ref(&e);
        EntityRef ref2(&e);

        assert(!(ref != ref2));
    }

    {
        // Check the comparison operator
        Entity e("1", 1);
        Entity e2("2", 2);
        EntityRef ref(&e);
        EntityRef ref2(&e2);

        assert(ref != ref2);
    }
#endif

    {
        // Check the less than operator
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(e);

        assert(!(ref < ref2) && !(ref2 < ref));
    }

    {
        // Check the less than operator
        Entity * e = new Entity("1", 1);
        Entity * e2 = new Entity("2", 2);
        EntityRef ref(e);
        EntityRef ref2(e2);

        assert(ref < ref2 || ref2 < ref);
    }

    {
        // Check the assignment operator
        Entity * e = new Entity("1", 1);
        EntityRef ref;

        ref = EntityRef(e);

        assert(ref.get() == e);
    }

    {
        // Check that destroying the Entity makes the reference null.
        Entity e("1", 1);
        Entity * container = new Entity("2", 2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_location.m_loc = container;
        // Make sure the container has a contains structure, as destroy
        // requires it.
        container->m_contains = new LocatedEntitySet;
        // Increment the refcount on the container, else the tested Entity's
        // destructor will delete it.
        container->incRef();

        EntityRef ref(&e);

        assert(ref.get() == &e);
        e.destroy();
        assert(ref.get() == 0);
    }

    checkSignal();
}
