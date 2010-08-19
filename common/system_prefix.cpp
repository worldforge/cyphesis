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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "compose.hpp"
#include "globals.h"
#include "log.h"
#include "system.h"

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <cassert>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif // HAVE_WINDOWS_H

static const bool debug_flag = false;

void getinstallprefix()
{
#ifdef HAVE_WINDOWS_H
    HKEY hKey;

    long res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                            "Software\\WorldForge\\Cyphesis\\Settings",
                            0, KEY_READ, &hKey);

    if (res != ERROR_SUCCESS)
    {
        log(CYLOG_ERROR, "No install key for cyphesis");
        return;
    }

    unsigned long type=REG_SZ, size=1024;
    char path[1024]="";

    res = RegQueryValueEx(hKey, "Path", NULL, &type, (LPBYTE)&path[0], &size);

    if (res != ERROR_SUCCESS)
    {
        log(CYLOG_ERROR, "No install key for cyphesis");
    } else {
        log(INFO, String::compose("Got %1", path));

        etc_directory = String::compose("%1/etc", path);
        var_directory = String::compose("%1/var", path);
    }

    RegCloseKey(hKey);
#endif // HAVE_WINDOWS_H
}
