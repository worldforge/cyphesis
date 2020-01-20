// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

#include "common/globals.h"

#include "common/log.h"

#include <cassert>

int main(int argc, char ** argv)
{
    reportVersion(argv[0]);

    showUsage(argv[0], USAGE_SERVER);
    showUsage(argv[0], USAGE_CLIENT);
    showUsage(argv[0], USAGE_CYCMD);
    showUsage(argv[0], USAGE_DBASE);

    showUsage(argv[0], 0);
    showUsage(argv[0], -1);

    showUsage(argv[0], USAGE_SERVER, 0);
    showUsage(argv[0], USAGE_SERVER, "");
    showUsage(argv[0], USAGE_SERVER, "test usage string");

    return 0;
}

// stubs
#include "../stubs/common/stublog.h"


void getinstallprefix()
{
}

namespace consts {
  const char * version = "unit_test";
}
