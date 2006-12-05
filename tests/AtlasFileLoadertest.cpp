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

// $Id: AtlasFileLoadertest.cpp,v 1.1 2006-12-05 20:44:52 alriddoch Exp $

#include "common/AtlasFileLoader.h"

#include <Atlas/Message/Element.h>

int main()
{
    {
        // Test constructor
        Atlas::Message::MapType data;
        AtlasFileLoader loader("foo", data);
    }

    {
        // Test destructor
        Atlas::Message::MapType data;
        AtlasFileLoader * loader = new AtlasFileLoader("foo", data);

        delete loader;
    }

    {
        // Test isOpen
        Atlas::Message::MapType data;
        AtlasFileLoader loader("foo", data);

        assert(!loader.isOpen());
    }

    {
        // Test count
        Atlas::Message::MapType data;
        AtlasFileLoader loader("foo", data);

        assert(loader.count() == 0);
        assert(loader.count() == data.size());
    }

    return 0;
}
