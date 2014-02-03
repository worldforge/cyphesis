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

namespace boost {
namespace asio {
class io_service;
}
}

/// \brief Base class for all classes for handling socket communication.
/// \ingroup ServerSockets
class CommSocket {
  protected:
    explicit CommSocket(boost::asio::io_service& io_service);
    CommSocket(const CommSocket &) = delete;
    CommSocket & operator=(const CommSocket &) = delete;
  public:
    /// Reference to the main IO service.
    boost::asio::io_service& m_io_service;

    /// \brief Destructor.
    virtual ~CommSocket();

    /// \brief Disconnect cleanly
    ///
    /// Politely inform the far end that we are done. This is normally done
    /// by shutting down the TCP connection
    virtual void disconnect() = 0;

    /// \brief Flush the socket
    virtual int flush() = 0;
};

#endif // COMMON_COMM_SOCKET_H
