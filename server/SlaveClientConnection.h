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

#ifndef SERVER_SLAVE_CLIENT_CONNECTION_H
#define SERVER_SLAVE_CLIENT_CONNECTION_H

#include "common/Identified.h"

class CommClient;
class ServerRouting;

/// \brief Class representing connections from a client to a cyphesis instance
/// running as an AI slave.
///
/// This behaves much like Connection, but handles the different behavoir
/// required because cyphesis is not the authoritative point for logins.
class SlaveClientConnection : public Router {
  public:
    /// \brief network object the client is connected to
    CommClient & m_commClient;
    /// \brief core server object
    ServerRouting & m_server;

    SlaveClientConnection(const std::string &, CommClient &, ServerRouting &);
    virtual ~SlaveClientConnection();

    virtual void operation(const Operation &, OpVector &);
};

#endif // SERVER_SLAVE_CLIENT_CONNECTION_H
