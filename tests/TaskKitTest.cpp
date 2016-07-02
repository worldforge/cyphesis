// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "common/TaskKit.h"

#include <cassert>

class TestTaskKit : public TaskKit {
  public:
    virtual ~TestTaskKit() { }

    virtual int checkTarget(LocatedEntity * target) { return 0; }

    virtual Task * newTask(LocatedEntity & chr) { return 0; }
};

int main()
{
    TaskKit * k = new TestTaskKit;

    assert(k->m_scriptFactory == 0);
    assert(k->target() == 0);

    delete k;

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}
