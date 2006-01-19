// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "rulesets/Container.h"

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;

    StdContainer sc;

    assert(sc.size() == 0);
    assert(sc.empty());

    StdContainer::const_iterator I = sc.begin();
    StdContainer::const_iterator Iend = sc.end();
    for (; I != Iend; ++I) {
        abort(); // There should be nothing in this container
    }

    Entity * e = (Entity *)23;

    sc.insert(e);

    assert(sc.size() != 0);
    assert(!sc.empty());

    int i = 0;

    for (I = sc.begin(); I != Iend; ++I) {
        Entity * ent = *I;
        assert(ent != 0);
        ++i;
        std::cout << "Ptr: " << ent << std::endl << std::flush;
    }

    assert(i != 0);

    return ret;
}
