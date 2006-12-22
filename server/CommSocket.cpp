// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: CommSocket.cpp,v 1.7 2006-12-22 02:14:45 alriddoch Exp $

#include "CommSocket.h"

/// \defgroup ServerSockets Server Socket Classes
///
////Socket handling classes for the server code.
/// Socket classes which inherit from CommSocket are used by the cyphesis
/// server class CommServer to communicate with other parts of a WorldForge
/// system using network sockets. All classes have a common interface which
/// allows CommServer to monitor these sockets for activity or events, and
/// notify the object when action is required.

/// \brief Constructor for socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}
