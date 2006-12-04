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

// $Id: EntityReftest.cpp,v 1.1 2006-12-04 00:50:39 alriddoch Exp $

#include "modules/EntityRef.h"

#include "rulesets/Entity.h"

#include <cassert>

int main()
{
    {
        // Check the default constructor
        EntityRef ref;

        assert(ref.get() == 0);
    }

    {
        // Check the initialising constructor
        Entity e("1", 1);
        EntityRef ref(&e);

        assert(ref.get() == &e);
    }

    {
        // Check the copy constructor
        Entity e("1", 1);
        EntityRef ref(&e);
        EntityRef ref2(ref);

        assert(ref2.get() == &e);
    }

    {
        // Check the assignment operator
        Entity e("1", 1);
        EntityRef ref;

        ref = EntityRef(&e);

        assert(ref.get() == &e);
    }

    {
        // Check that destroying the Entity makes the reference null.
        Entity e("1", 1);
        Entity container("2", 2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_location.m_loc = &container;
        // Increment the refcount on the container, else the tested Entity's
        // destructor will delete it.
        container.incRef();

        EntityRef ref(&e);

        assert(ref.get() == &e);
        e.destroy();
        assert(ref.get() == 0);
    }
}
