// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

#ifndef SERVER_INTER_SERVER_CLIENT_H
#define SERVER_INTER_SERVER_CLIENT_H

#include "InterServerConnection.h"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Entity::RootEntity;

/// \brief Class to implement a client for inter-server communication
class InterServerClient {
  protected:
    InterServerConnection & m_connection;
    
    /// \brief Send an operation to the server, and wait for a reply
    ///
    /// @param op Operation to be sent
    /// @param res Result with correct refno is returned here
    int sendAndWaitReply(const Operation &, OpVector &);
    
  public:
    InterServerClient(InterServerConnection&);

    /// \brief Connect to a cyphesis instance using a network socket
    ///
    /// @param server The hostname/IP to connect to
    /// @param port The port to connect on
    int connect(const std::string & server, int port = 6767);
    
    /// \brief Login to another cyphesis instance using the specified credentials
    ///
    /// @param username The username to use
    /// @param password The password to use
    int login(const std::string & username, const std::string & password);

    /// \brief Send an operation to the server from this account
    ///
    /// @param op Operation to be sent
    void send(const Operation & op);
    
    /// \brief Inject an entity into a remote cyphesis instance
    ///
    /// @param entity The entity to be inserted
    std::string injectEntity(const RootEntity & entity);
};

#endif // SERVER_INTER_SERVER_CLIENT_H
