// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_CLIENT_H
#define SERVER_COMM_CLIENT_H

#include "CommSocket.h"

#include <Atlas/Objects/Decoder.h>

#include <skstream/skstream.h>

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

class CommClient : public Atlas::Objects::Decoder, public CommSocket {
  protected:
    tcp_socket_stream clientIos;
    Atlas::Codec<std::iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    Atlas::Net::StreamAccept * accept;
    Connection & connection;
    bool reading;

    bool negotiate();

    virtual void UnknownObjectArrived(const Atlas::Message::Object&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Login & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Logout & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Create & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Imaginary & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Move & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Set & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Touch & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Look & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Talk & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Get & op);

  public:
    CommClient(CommServer &, int fd, Connection &);
    virtual ~CommClient();

    int getFd() const { return clientIos.getSocket(); }
    bool eof() { return clientIos.peek() == EOF; }
    bool isOpen() const { return clientIos.is_open(); }
    bool timeout() { return clientIos.timeout(); }
    bool online() const { return (encoder != NULL); }
    void close() { clientIos.close(); }

    bool read();
    void send(const Atlas::Objects::Operation::RootOperation &);
    void message(const Atlas::Objects::Operation::RootOperation &);
    void setup();
};

#endif // SERVER_COMM_CLIENT_H
