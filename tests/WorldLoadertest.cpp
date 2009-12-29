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

// $Id$

#include "tools/WorldLoader.h"

#include "common/compose.hpp"

#include <Atlas/Objects/RootOperation.h>

#include <cassert>

const std::string data_path = TESTDATADIR;

int main()
{
    WorldLoader * wl = new WorldLoader("23", "42");
    delete wl;

    OpVector res;
    wl = new WorldLoader("23", "42");
    wl->setup(String::compose("%1/no_such_file", data_path), res);
    assert(wl->isComplete());
    assert(res.empty());
    delete wl;

    res.clear();
    wl = new WorldLoader("23", "42");
    wl->setup(String::compose("%1/world.xml", data_path), res);
    assert(!wl->isComplete());
    assert(!res.empty());
}
