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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include "common/compose.hpp"
#include "common/log.h"

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

extern "C" {
#ifdef HAVE_WINSOCK_H
    #include <winsock.h>
#endif // HAVE_WINSOCK_H
}

unsigned int security_check()
{
#ifdef HAVE_GETUID
    if (getuid() == 0 || geteuid() == 0) {
        log(CYLOG_ERROR, "Running cyphesis as the superuser is dangerous.");
        return 0;
    }
#endif // HAVE_GETUID
    return SECURITY_OKAY;
}

std::string create_session_username()
{
#ifdef HAVE_GETUID
    return String::compose("admin_%1_%2", getpid(), getuid());
#elif defined(_WIN32)
    return String::compose("admin_%1_%2", GetCurrentProcessId(), 23);
#else // _WIN32
    return String::compost("admin_%1", ::rand());
#endif // _WIN32
}
