// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
