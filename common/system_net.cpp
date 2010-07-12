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

#include "system.h"

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <cassert>

extern "C" {
#ifdef HAVE_SYS_UTSNAME_H
    #include <sys/utsname.h>
#endif // HAVE_SYS_UTSNAME_H
    #include <sys/types.h>
}

#ifdef HAVE_WINSOCK_H
#include <winsock2.h>
#endif // HAVE_WINSOCK_H

static const bool debug_flag = false;

const std::string get_hostname()
{
#ifndef HAVE_UNAME
    char hostname_buf[256];

    if (gethostname(hostname_buf, 256) != 0) {
        return "UNKNOWN";
    }
    return std::string(hostname_buf);
#else // HAVE_UNAME
    struct utsname host_ident;
    if (uname(&host_ident) != 0) {
        return "UNKNOWN";
    }
    return std::string(host_ident.nodename);
#endif // HAVE_UNAME
}
