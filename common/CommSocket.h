// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef COMMON_COMM_SOCKET_H
#define COMMON_COMM_SOCKET_H

#include "common/io_context.h"


/// \defgroup ServerSockets Server Socket Classes
///
////Socket handling classes for the server code.
/// Socket classes which inherit from CommSocket are to communicate with other
/// parts of a WorldForge system using network sockets.
/// \brief Constructor for socket object.
///
/// @param io_context Reference to the object that manages all socket communication.
class CommSocket {
  protected:
    explicit CommSocket(boost::asio::io_context& io_context)
    : m_io_context(io_context), m_active(true){
    }

  public:

    CommSocket(const CommSocket &) = delete;
    CommSocket & operator=(const CommSocket &) = delete;

    /// Reference to the main IO service.
    boost::asio::io_context& m_io_context;

    bool m_active;

    /// \brief Destructor.
    virtual ~CommSocket() = default;

    /// \brief Disconnect cleanly
    ///
    /// Politely inform the far end that we are done. This is normally done
    /// by shutting down the TCP connection
    virtual void disconnect() = 0;

    /// \brief Flush the socket
    virtual int flush() = 0;
};

#endif // COMMON_COMM_SOCKET_H
