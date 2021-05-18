/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../PropertyCoverage.h"
#include "../../stubs/common/stubcustom.h"
#include "../../stubs/common/stubglobals.h"
#include "../../stubs/rules/stubQuaternionProperty.h"
#include "../../stubs/rules/simulation/stubOgreMeshDeserializer.h"
#include "../../stubs/rules/stubBBoxProperty.h"
#include "../../stubs/common/stubTypeNode.h"

#include "rules/simulation/GeometryProperty.h"

int main()
{
    GeometryProperty ap;

    PropertyChecker<GeometryProperty> pc(ap);

    // Coverage is complete, but it wouldn't hurt to add some bad data here.

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

#include "../../TestWorld.h"



#include "../../stubs/common/stubAssetsManager.h"
#include "../../stubs/common/stubInheritance.h"
