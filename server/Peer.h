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

// $Id$

#ifndef SERVER_PEER_H
#define SERVER_PEER_H

#include "common/Identified.h"

class CommClient;
class ServerRouting;

/// \brief Class represening connections from another server that is peered to
/// to this one
///
/// This is the main point of dispatch for any operation from the peer.
class Peer : public Router {
  protected:
    
  public:
    /// The client socket used to connect to the peer.
    CommClient & m_commClient;
    /// The server routing object of this server.
    ServerRouting & m_server;

    Peer(CommClient & client, ServerRouting & svr,
         const std::string & addr, const std::string & id);
    virtual ~Peer();

    virtual void operation(const Operation &, OpVector &);
};

#endif // SERVER_PEER_H
