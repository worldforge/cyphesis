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

#include "common/Monitors.h"
#include "common/Variable.h"

#include <iostream>
#include <sstream>

#include <cassert>

int main()
{
    Monitors m;

    int foo = 7;
    std::stringstream ss;

    m.watch("foo", std::make_unique<Variable<int>>(foo));

    m.insert("bar", 3);
    m.insert("mim", 3.f);
    m.insert("qux", "three");

    // positive check
    assert( m.readVariable("foo",ss) == 0 );
    assert( ss.str().compare("7") == 0 );

    // negative check
    ss.clear();
    assert(m.readVariable("nonexistent",ss) != 0);

    m.send(std::cout);

    return 0;
}

namespace Atlas { namespace Objects { namespace Operation {
int MONITOR_NO = -1;
} } }
