// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: ClientConnection.h,v 1.21 2006-10-26 00:48:01 alriddoch Exp $

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Codec.h>

#include <skstream/skstream.h>

#include <deque>

namespace Atlas {
  class Codec;
  namespace Objects {
    class ObjectsEncoder;
  }
}

/// \brief Class to handle socket connection to a cyphesis server from an
/// an admin client
class ClientConnection : public Atlas::Objects::ObjectsDecoder {
  private:
    bool reply_flag;
    bool error_flag;
    int client_fd;
    tcp_socket_stream ios;
    Atlas::Codec * codec;
    Atlas::Objects::ObjectsEncoder * encoder;
    Atlas::Objects::Root reply;
    int serialNo;

    std::deque<Atlas::Objects::Operation::RootOperation> operationQueue;

    template<class O>
    void push(const O &);

    void operation(const Atlas::Objects::Operation::RootOperation&);

    int negotiate();

    virtual void objectArrived(const Atlas::Objects::Root &);

    void errorArrived(const Atlas::Objects::Operation::RootOperation &);
    void infoArrived(const Atlas::Objects::Operation::RootOperation &);

  public:
    ClientConnection();
    ~ClientConnection();

    int read();
    int connectLocal(const std::string &);
    int connect(const std::string &);
    void login(const std::string &, const std::string &);
    void create(const std::string &, const std::string &);
    int wait();
    void send(const Atlas::Objects::Operation::RootOperation & op);

    int peek() {
        return ios.peek();
    }
    int eof() {
        return ios.eof();
    }
    int get_fd() {
        return client_fd;
    }
    const Atlas::Objects::Root & getReply() {
        return reply;
    }

    void poll(int timeOut = 0);
    Atlas::Objects::Operation::RootOperation pop();
    bool pending();
};

#endif // CLIENT_CONNECTION_H
