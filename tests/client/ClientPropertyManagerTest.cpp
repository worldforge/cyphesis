// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "client/ClientPropertyManager.h"

#include <cassert>

int main()
{
    {
        ClientPropertyManager * cpm = new ClientPropertyManager;
        delete cpm;
    }

    {
        ClientPropertyManager * cpm = new ClientPropertyManager;
        assert(cpm->addProperty("foo"));
        delete cpm;
    }

    return 0;
}

#include "../stubs/rules/stubBBoxProperty.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/rules/stubScaleProperty.h"
#include "../stubs/rules/stubSolidProperty.h"
#include "../stubs/rules/stubVector3Property.h"
#include "../stubs/rules/stubPhysicalProperties.h"
#include "../stubs/common/stubProperty.h"
