// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMM_CLIENT_H
#define COMM_CLIENT_H

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
    int client_fd;
    //ofstream log_file;
    sockbuf client_buf;
    iostream client_ios;
    Atlas::Codec<iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    Connection * connection;

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
    CommServer * server;

    CommClient(CommServer * svr, int fd, int port) :
		client_fd(fd), client_buf(fd), client_ios(&client_buf),
                server(svr) {
        //if (consts::debug_level>=1) {
            //char * log_name = "log.log";
            //log_file.open(log_name);
        //}
    }
    virtual ~CommClient();

    int read() {
        if (client_ios) {
            codec->Poll();
            return(0);
        } else {
            return(-1);
        }
    }

    void send(const RootOperation * op) {
        if (op) {
            encoder->StreamMessage(op);
            client_ios << flush;
        }
    }

    int peek() { return client_ios.peek(); }
    int eof() { return client_ios.eof(); }
    int get_fd() { return client_fd; }

    void message(const RootOperation &);
    int setup();
};

#endif /* COMM_CLIENT_H */
