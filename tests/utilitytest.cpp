// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "common/utility.h"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

int main()
{
    Atlas::Objects::Operation::RootOperation op;
    Atlas::Message::MapType map;

    assert(utility::Object_asOperation(map, op) == false);

    map["objtype"] = "op";
    map["parents"] = Atlas::Message::ListType(1, "root_operation");

    assert(utility::Object_asOperation(map, op) == true);

    return 0;
}
