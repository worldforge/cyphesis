// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

// $Id: logtest.cpp,v 1.4 2006-10-26 00:48:16 alriddoch Exp $

#include "common/log.h"
#include "common/globals.h"

#include <cassert>

int main()
{
    assert(daemon_flag == false);

    log(INFO, "Info log message.");
    log(SCRIPT, "Script log message.");
    log(NOTICE, "Notice log message.");
    log(WARNING, "Warning log message.");
    log(SCRIPT_ERROR, "Script Error log message.");
    log(CRITICAL, "Critical log message.");

    return 0;
}
