// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMM_CLIENT_H
#define COMM_CLIENT_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Codec.h>

#include <fstream>

#include <common/const.h>

class CommServer;
class Connection;

class sockbuf : public filebuf {
  public:
    sockbuf() { }
    sockbuf(int fd) : filebuf(fd) { }
    virtual streampos sys_seek(streamoff, _seek_dir) { return streampos(); }
};

class CommClient : Atlas::Objects::Decoder {
  public:
    CommServer & commServer;

  private:
    int clientFd;
    sockbuf clientBuf;
    iostream clientIos;
    Atlas::Codec<iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    Connection & connection;

  protected:
    virtual void UnknownObjectArrived(const Atlas::Message::Object&);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Login & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Create & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Move & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Set & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Touch & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Look & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Talk & op);
    virtual void ObjectArrived(const Atlas::Objects::Operation::Get & op);

  public:
    CommClient(CommServer & svr, int fd, int port);
    virtual ~CommClient();

    void read() {
        codec->Poll();
    }

    void send(const RootOperation * op) {
        if (op) {
            encoder->StreamMessage(op);
            clientIos << flush;
        }
    }

    int peek() { return clientIos.peek(); }
    int eof() { return clientIos.eof(); }
    int getFd() { return clientFd; }

    void message(const RootOperation &);
    bool setup();
};

#endif // COMM_CLIENT_H
