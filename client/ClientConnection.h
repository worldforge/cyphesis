// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <common/operations.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codec.h>

#include <skstream/skstream.h>

#include <deque>

namespace Atlas { namespace Objects {
    class Encoder;
} }

class ClientConnection : public Atlas::Objects::Decoder {
  private:
    bool reply_flag;
    bool error_flag;
    int client_fd;
    tcp_socket_stream ios;
    Atlas::Codec<std::iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    Atlas::Message::Object::MapType reply;
    int serialNo;

    std::deque<Atlas::Objects::Operation::RootOperation *> operationQueue;

    template<class O>
    void push(const O &);

    void operation(const Atlas::Objects::Operation::RootOperation&);

    virtual void ObjectArrived(const Atlas::Objects::Operation::Error&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Info&);

    virtual void ObjectArrived(const Atlas::Objects::Operation::Action&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Appearance&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Combine&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Communicate&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Create&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Delete&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Disappearance&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Divide&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Feel&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Get&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Imaginary&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Listen&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Login&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Logout&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Look&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Move&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Perceive&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Perception&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::RootOperation&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Set&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Sight&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Smell&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Sniff&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Sound&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Talk&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Touch&);

  public:
    ClientConnection();
    ~ClientConnection();

    int read();
    bool connect(const std::string &);
    bool login(const std::string &, const std::string &);
    bool create(const std::string &, const std::string &);
    bool wait();
    void send(Atlas::Objects::Operation::RootOperation & op);

    int peek() {
        return ios.peek();
    }
    int eof() {
        return ios.eof();
    }
    int get_fd() {
        return client_fd;
    }
    const Atlas::Message::Object::MapType & getReply() {
        return reply;
    }

    int setup();
    void poll(int timeOut = 0);
    RootOperation * pop();
    bool pending();
};

#endif // CLIENT_CONNECTION_H
