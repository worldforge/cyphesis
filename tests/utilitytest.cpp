// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "common/utility.h"

#include <Atlas/Objects/Operation/RootOperation.h>

#include <cassert>

int main()
{
    RootOperation op;
    Element::MapType map;

    assert(utility::Object_asOperation(map, op) == false);

    map["objtype"] = "op";

    assert(utility::Object_asOperation(map, op) == true);

    Atlas::Objects::Root * r = utility::Object_asRoot(map);

    assert(r != 0);

    delete r; // Supresses warning about unused variable, and eliminates leak.

    return 0;
}
