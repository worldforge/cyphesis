// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

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

    void operation(const Atlas::Objects::Operation::RootOperation &);

    virtual void objectArrived(const Atlas::Objects::Root & obj);

    virtual void idle(time_t t);
  public:
    CommClient(CommServer &, int fd, BaseEntity &);
    CommClient(CommServer &, BaseEntity &);
    virtual ~CommClient();

    void disconnect() { m_clientIos.shutdown(); }

    void setup();
    void send(const Atlas::Objects::Operation::RootOperation &);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();
};

#endif // SERVER_COMM_CLIENT_H
