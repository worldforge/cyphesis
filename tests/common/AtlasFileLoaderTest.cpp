// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#include "common/AtlasFileLoader.h"

#include "common/log.h"

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include <Atlas/Objects/Factories.h>

int main()
{
    Atlas::Objects::Factories factories;
    {
        // Test constructor
        std::map<std::string, Atlas::Objects::Root> data;
        AtlasFileLoader loader(factories, "foo", data);
    }

    {
        // Test destructor
        std::map<std::string, Atlas::Objects::Root> data;
        AtlasFileLoader * loader = new AtlasFileLoader(factories, "foo", data);

        delete loader;
    }

    {
        // Test isOpen
        std::map<std::string, Atlas::Objects::Root> data;
        AtlasFileLoader loader(factories, "foo", data);

        assert(!loader.isOpen());
    }

    {
        // Test count
        std::map<std::string, Atlas::Objects::Root> data;
        AtlasFileLoader loader(factories, "foo", data);

        assert(loader.count() == 0);
        assert((unsigned int)loader.count() == data.size());
    }

    return 0;
}

#include "../stubs/common/stublog.h"
