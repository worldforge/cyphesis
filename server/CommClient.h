// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_CLIENT_H
#define SERVER_COMM_CLIENT_H

#include "CommSocket.h"

#include <Atlas/Objects/Decoder.h>

#include <skstream/skstream.h>

#include <deque>

namespace Atlas {
  template <class Stream> class Codec;
  namespace Objects {
    class Encoder;
  }
  namespace Net {
    class StreamAccept;
  }
}

class Connection;

/// \brief Base class for Atlas clients connected to the server.
///
/// Used by subclasses to handle remote TCP clients and local UNIX clients.
class CommClient : public Atlas::Objects::Decoder, public CommSocket {
  public:
    /// \brief STL deque of pointers to operation objects.
    typedef std::deque<const Atlas::Objects::Operation::RootOperation *> DispatchQueue;
  protected:
    /// \brief C++ iostream compatible socket object handling the socket IO.
    tcp_socket_stream m_clientIos;
    /// \brief Queue of operations that have been decoded by not dispatched.
    DispatchQueue m_opQueue;
    /// \brief Atlas codec that handles encoding and decoding traffic.
    Atlas::Codec<std::iostream> * m_codec;
    /// \brief high level encoder passes data to the codec for transmission.
    Atlas::Objects::Encoder * m_encoder;
    /// \brief Atlas negotiator for handling codec negotiation.
    Atlas::Net::StreamAccept * m_accept;
    /// \brief Server side object for handling connection level operations.
    Connection & m_connection;

    /// \brief Handle socket data related to codec negotiation.
    bool negotiate();

    /// \brief Add an operation to the queue.
    template <class OpType>
    void queue(const OpType &);

    bool timeout() { return m_clientIos.timeout(); }

    void message(const Atlas::Objects::Operation::RootOperation &);

    virtual void unknownObjectArrived(const Atlas::Message::Element&);
    virtual void objectArrived(const Atlas::Objects::Operation::Login & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Logout & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Create & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Imaginary & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Move & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Set & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Touch & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Look & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Talk & op);
    virtual void objectArrived(const Atlas::Objects::Operation::Get & op);

  public:
    CommClient(CommServer &, int fd, Connection &);
    virtual ~CommClient();

    void close() { m_clientIos.close(); }

    void setup();
    void send(const Atlas::Objects::Operation::RootOperation &);

    int getFd() const;
    bool isOpen() const;
    bool eof();
    bool read();
    void dispatch();
};

#endif // SERVER_COMM_CLIENT_H
