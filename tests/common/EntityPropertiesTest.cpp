// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "../PropertyCoverage.h"

#include "common/Property.h"
#include "common/types.h"

static void test_Property_IdList()
{
    Property<IdList> ap;

    PropertyChecker<Property<IdList>> pc(ap);

    // Coverage is complete, but it wouldn't hurt to add some bad data here.

    pc.basicCoverage();
}

int main()
{
    test_Property_IdList();
    return 0;
}

// stubs

#include "../TestWorld.h"



void idListasObject(const IdList & l, Atlas::Message::ListType & ol)
{
}

int idListFromAtlas(const Atlas::Message::ListType & l, IdList & ol)
{
    return 0;
}
