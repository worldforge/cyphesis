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

#include "tools/OperationMonitor.h"

#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>

#include <cassert>

int main()
{
    {
        ClientTask * tf = new OperationMonitor;

        delete tf;
    }

    {
        ClientTask * tf = new OperationMonitor;

        OpVector ret;
        tf->setup("df954cce-957b-49ac-80e1-be75d83382a1", ret);
        assert(ret.empty());

        delete tf;
    }

}

// stubs

#include "common/log.h"
#include "../stubs/common/stublog.h"
