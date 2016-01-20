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

#include "common/ClientTask.h"

#include <cassert>

class TestClientTask : public ClientTask {
  public:
    virtual void setup(const std::string & arg, OpVector & res) {
    }
    /// \brief Handle an operation from the server
    virtual void operation(const Operation &, OpVector & res) {
    }
};

int main()
{
    ClientTask * ct = new TestClientTask();

    bool complete = ct->isComplete();
    assert(!complete);

    delete ct;
}
