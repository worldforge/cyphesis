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

/// \brief Class to implement a client for inter-server communication
class InterServerClient {
  protected:
    InterServerConnection & m_connection;

    int sendAndWaitReply(const Operation &, OpVector &);
  public:
    InterServerClient(InterServerConnection&);

	/// \brief Connect to a remote server using a network socket
	int connect(const std::string & server, int port = 6767) {
		return m_connection.connect(server, port);
	}
	
	/// \brief Login to a remote server using the specified credentials
	int login(const std::string & username, const std::string & password) {
		return m_connection.login(username, password);
	}

	/// \brief Send an operation to the connected server
    void send(const Operation & op);
};

#endif // SERVER_INTER_SERVER_CLIENT_H
