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

// $Id: SlaveClientConnection.cpp,v 1.6 2008-01-26 17:43:22 alriddoch Exp $

#include "SlaveClientConnection.h"

/// \brief SlaveClientConnection constructor
///
/// @param id identifier of the connection
/// @param client network object the client is connected to
/// @param svr core server object
SlaveClientConnection::SlaveClientConnection(const std::string & id,
                                             CommClient & client,
                                             ServerRouting & svr) :
                       Identified(id, -1),
                       OOGThing(id, -1), m_commClient(client), m_server(svr)
{
}

SlaveClientConnection::~SlaveClientConnection()
{
}
