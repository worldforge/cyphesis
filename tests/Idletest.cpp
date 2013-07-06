// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/Idle.h"

#include "server/CommServer.h"

#include <cassert>

class TestIdler : public Idle
{
  public:
    TestIdler(CommServer & svr) : Idle(svr) { }
    ~TestIdler() { }

    virtual void idle(time_t) { }
};

int main()
{
    {
        CommServer cs;

        TestIdler ti(cs);
    }

    return 0;
}

// stubs

CommServer::CommServer()
{
}

CommServer::~CommServer()
{
}
