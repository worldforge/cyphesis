// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "common/const.h"

#include <string>

#include <cassert>

int main()
{
    int debug_thinking;
    debug_thinking = consts::debug_thinking;
    assert(debug_thinking == 0 || debug_thinking == 1);
    int debug_level;
    debug_level = consts::debug_level;
    assert(debug_level == 0 || debug_level == 1);

    float time_multiplier;
    time_multiplier = consts::time_multiplier;
    assert(time_multiplier > 0);
    float basic_tick;
    basic_tick = consts::basic_tick;
    assert(basic_tick > 0);
    float square_basic_tick;
    square_basic_tick = consts::square_basic_tick;
    assert(square_basic_tick > 0);

    const char * rootWorldId;
    rootWorldId = consts::rootWorldId;
    assert(rootWorldId != 0);
    std::string rootWorldIds;
    rootWorldIds = consts::rootWorldId;
    assert(rootWorldIds.size() > 0);

    const char * version;
    version = consts::version;
    assert(version != 0);
    std::string versions;
    versions = consts::version;
    assert(versions.size() > 0);

    return 0;
}
