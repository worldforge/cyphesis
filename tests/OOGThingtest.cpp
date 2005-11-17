// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "EntityExerciser.h"
#include "allOperations.h"

#include "common/OOGThing.h"

#include <cassert>

class OOGThingTest : public OOGThing {
  public:
    OOGThingTest() : OOGThing("1", 1) { }
};

int main()
{
    OOGThingTest be;

    EntityExerciser<OOGThing> ee(be);

    ee.runOperations();

    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    ee.runOperations();

    return 0;
}
