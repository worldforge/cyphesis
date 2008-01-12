// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: OOGThingtest.cpp,v 1.9 2008-01-12 22:41:12 alriddoch Exp $

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
    {
        OOGThingTest te;
    }

    {
        OOGThingTest * te = new OOGThingTest;

        delete te;
    }

    {
        OOGThingTest te;
        Operation op;
        OpVector res;

        te.OtherOperation(op, res);
        assert(!res.empty());
        Operation res1 = res.front();
        assert(res1->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
    }

    OOGThingTest be;

    EntityExerciser<OOGThing> ee(be);

    ee.runOperations();

    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    ee.runOperations();

    return 0;
}
