// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Alistair Riddoch and Aloril

/*
        InheritanceClient.h
        ----------------
        begin           : 1999.12.18
        copyright       : (C) 1999 by Aloril
        email           : aloril@iki.fi
*/

#ifndef InheritanceClient_h
#define InheritanceClient_h

#include <Atlas/Objects/Decoder.h>

#include <string.h>
#include <stdio.h>
#include <list>

#include <skstream.h>

class InheritanceClient : public Atlas::Objects::Decoder
{
  private:
    int cli_fd;

    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<iostream> * codec;
    client_socket_stream ios;

  protected:
    void UnknownObjectArrived(const Atlas::Message::Object&);
    void ObjectArrived(const Atlas::Objects::Operation::Info&);
    void ObjectArrived(const Atlas::Objects::Operation::Error&);
  public:
    InheritanceClient() : encoder(NULL), codec(NULL) { }
     
    void send(Atlas::Objects::Operation::RootOperation *);
    int connect();
    void get(const std::string &);
    void start();
    void loop();
};

#endif
