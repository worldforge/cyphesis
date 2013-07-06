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


#ifndef SERVER_COMM_CLIENT_H
#define SERVER_COMM_CLIENT_H

#include "CommStreamClient.h"
#include "Idle.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <deque>

namespace Atlas {
  class Codec;
  namespace Objects {
    class ObjectsEncoder;
  }
  class Negotiate;
}

class Link;

/// \brief Base class for Atlas clients connected to the server.
///
/// Used by subclasses to handle remote TCP clients and local UNIX clients.
/// \ingroup ServerSockets
template <class StreamT>
class CommClient : public Atlas::Objects::ObjectsDecoder,
                   public CommStreamClient<StreamT>,
                   public Idle {
  public:
    /// \brief STL deque of pointers to operation objects.
    typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;
  protected:
    /// \brief Queue of operations that have been decoded by not dispatched.
    DispatchQueue m_opQueue;
    /// \brief Atlas codec that handles encoding and decoding traffic.
    Atlas::Codec * m_codec;
    /// \brief high level encoder passes data to the codec for transmission.
    Atlas::Objects::ObjectsEncoder * m_encoder;
    /// \brief Atlas negotiator for handling codec negotiation.
    Atlas::Negotiate * m_negotiate;
    /// \brief Server side object for handling connection level operations.
    Link * m_link;
    /// \brief Time connection was opened
    time_t m_connectTime;

    /// \brief Handle socket data related to codec negotiation.
    int negotiate();

    /// \brief Add an operation to the queue.
    template <class OpType>
    void queue(const OpType &);

    bool timeout() { return this->m_clientIos.timeout(); }

    int operation(const Atlas::Objects::Operation::RootOperation &);

    virtual void objectArrived(const Atlas::Objects::Root & obj);

    virtual void idle(time_t t);

    CommClient(CommServer &, const std::string &);
    CommClient(CommServer &, const std::string &, int fd);
  public:
    virtual ~CommClient();

    void setup(Link * connection);
    int send(const Atlas::Objects::Operation::RootOperation &);

    int read();
    void dispatch();
};

#endif // SERVER_COMM_CLIENT_H
