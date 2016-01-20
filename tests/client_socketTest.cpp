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

#include "common/sockets.h"

#include "common/globals.h"

int main()
{
    // Reference the globals to ensure they get linked.
    (void)client_socket_name;
    (void)python_socket_name;
    (void)slave_socket_name;

    return 0;
}

// stubs

const char * const CYPHESIS = "cyphesis";

unixsock_config_register::unixsock_config_register(std::string & var,
                                                   const char * section,
                                                   const char * setting,
                                                   const char * help,
                                                   const char * format)
{
}
