// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id: Peer.cpp,v 1.7 2007-09-22 15:34:05 alriddoch Exp $

#include "Peer.h"

#include "common/id.h"

/// \brief Constructor
///
/// @param client the client socket used to connect to the peer.
/// @param svr the server routing object of this server.
/// @param addr a string representation of the address of the peer.
/// @param id a string giving the indentifier of the peer connection.
Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id) :
      OOGThing(id, forceIntegerId(id)), m_commClient(client), m_server(svr)
{
}

Peer::~Peer()
{
}
