// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_CLIENT_H
#define SERVER_COMM_CLIENT_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>

#include <skstream/skstream.h>

#include <stdexcept>

class CommServer;
class Connection;

class ClientTimeOutException : public std::runtime_error {
  public:
    ClientTimeOutException() : std::runtime_error("Client write timeout") { }
    virtual ~ClientTimeOutException() throw() { }
};

class CommClient : Atlas::Objects::Decoder {
  public:
    CommServer & commServer;

  private:
    basic_socket_stream clientIos;
    Atlas::Codec<std::iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    Atlas::Net::StreamAccept * accept;
    Connection & connection;
    bool reading;

    bool negotiate();
  protected:
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
    CommClient(CommServer & svr, int fd, int port);
    virtual ~CommClient();

    int peek() { return clientIos.peek(); }
    int eof() { return clientIos.eof(); }
    int getFd() { return clientIos.getSocket(); }
    bool isOpen() { return clientIos.is_open(); }
    bool timeout() { return clientIos.timeout(); }
    bool online() { return (encoder != NULL); }
    void close() { clientIos.close(); }

    bool read();
    void send(const Atlas::Objects::Operation::RootOperation &);
    void message(const Atlas::Objects::Operation::RootOperation &);
    void setup();
};

#endif // SERVER_COMM_CLIENT_H
