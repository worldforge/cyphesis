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


#include "sockets.h"
#include "globals.h"

static const char * DEFAULT_CLIENT_SOCKET = "cyphesis.sock";
static const char * DEFAULT_PYTHON_SOCKET = "cypython.sock";
static const char * DEFAULT_SLAVE_SOCKET = "cyslave.sock";

UNIXSOCK_OPTION(client_socket_name, DEFAULT_CLIENT_SOCKET, CYPHESIS,
                "unixport", "Local listen socket for admin connections",
                "cyphesis_%1.sock")
UNIXSOCK_OPTION(python_socket_name, DEFAULT_PYTHON_SOCKET, CYPHESIS,
                "pythonport", "Local listen socket for python connections",
                "cypython_%1.sock")
UNIXSOCK_OPTION(slave_socket_name, DEFAULT_SLAVE_SOCKET, "slave", "unixport",
                "Local listen socket for admin connections to the slave server",
                "cyslave_%1.sock")
