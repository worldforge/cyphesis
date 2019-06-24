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

#include "server/IdleConnector.h"

#include <sigc++/functors/ptr_fun.h>

#include <cassert>

static bool trigger_called = false;

static void trigger()
{
    trigger_called = true;
}

int main()
{
    boost::asio::io_context s;
    IdleConnector * ic = new IdleConnector(s);

    ic->idling.connect(sigc::ptr_fun(trigger));

    assert(!trigger_called);

    s.run_one();

    assert(trigger_called);

    delete ic;

    return 0;
}

// stubs

