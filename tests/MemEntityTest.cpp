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

#include "rulesets/MemEntity.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/InternalProperties.h"
#include "common/Property_impl.h"

#include <cassert>

int main()
{
    MemEntity * me = new MemEntity("1", 1);

    delete me;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"

#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubSimpleProperty.h"
#include "stubs/rulesets/stubSolidProperty.h"
