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

// $Id: CommClient.h,v 1.54 2006-12-22 02:14:44 alriddoch Exp $

#ifndef SERVER_COMM_CLIENT_H
#define SERVER_COMM_CLIENT_H

#include "CommSocket.h"
#include "Idle.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <skstream/skstream.h>

#include <deque>

namespace Atlas {
  class Codec;
  namespace Objects {
    class ObjectsEncoder;
  }
  class Negotiate;
}

class BaseEntity;

/// \brief Base class for Atlas clients connected to the server.
///
/// Used by subclasses to handle remote TCP clients and local UNIX clients.
/// \ingroup ServerSockets
class CommClient : public Atlas::Objects::ObjectsDecoder, public CommSocket, public Idle {
  public:
    /// \brief STL deque of pointers to operation objects.
    typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;
  protected:
    /// \brief C++ iostream compatible socket object handling the socket IO.
    tcp_socket_stream m_clientIos;
    /// \brief Queue of operations that have been decoded by not dispatched.
    DispatchQueue m_opQueue;
    /// \brief Atlas codec that handles encoding and decoding traffic.
    Atlas::Codec * m_codec;
    /// \brief high level encoder passes data to the codec for transmission.
    Atlas::Objects::ObjectsEncoder * m_encoder;
    /// \brief Atlas negotiator for handling codec negotiation.
    Atlas::Negotiate * m_negotiate;
    /// \brief Server side object for handling connection level operations.
    BaseEntity & m_connection;
    /// \brief Time connection was opened
    time_t m_connectTime;

    /// \brief Handle socket data related to codec negotiation.
    int negotiate();

    /// \brief Add an operation to the queue.
    template <class OpType>
    void queue(const OpType &);

    bool timeout() { return m_clientIos.timeout(); }

    int operation(const Atlas::Objects::Operation::RootOperation &);

    virtual void objectArrived(const Atlas::Objects::Root & obj);

    virtual void idle(time_t t);
  public:
    CommClient(CommServer &, int fd, BaseEntity &);
    CommClient(CommServer &, BaseEntity &);
    virtual ~CommClient();

    void disconnect() { m_clientIos.shutdown(); }

    void setup();
    int send(const Atlas::Objects::Operation::RootOperation &);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();
};

#endif // SERVER_COMM_CLIENT_H
