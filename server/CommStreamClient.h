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


#ifndef SERVER_COMM_STREAM_CLIENT_H
#define SERVER_COMM_STREAM_CLIENT_H

#include "common/CommSocket.h"

/// \brief Base class for stream clients connected to the server.
///
/// \ingroup ServerSockets
template <class StreamT>
class CommStreamClient : public CommSocket {
  protected:
    /// \brief C++ iostream compatible socket object handling the socket IO.
    StreamT m_clientIos;

    CommStreamClient(CommServer &, int fd);
    CommStreamClient(CommServer &);

  public:
    virtual ~CommStreamClient();

    virtual int getFd() const;
    virtual bool isOpen() const;
    virtual bool eof();
    virtual void disconnect();
    virtual int flush();

};

#endif // SERVER_COMM_STREAM_CLIENT_H
