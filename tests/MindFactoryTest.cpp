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

#include "rulesets/MindFactory.h"

#include "rulesets/BaseMind.h"

#include <cassert>

int main()
{
    MindFactory * mf = new MindFactory;

    mf->newMind("1", 1);

    mf->newMind("2", 2);

    return 0;
}

// stubs

#include "stubs/rulesets/stubMemMap.h"
#include "stubs/rulesets/stubBaseMind.h"
#include "stubs/rulesets/stubMemEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"

void WorldTime::initTimeInfo()
{
}

DateTime::DateTime(int t)
{
}
