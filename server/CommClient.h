// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
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

using namespace Atlas;

class CommClient : Objects::Decoder {
    int client_fd;
    //ofstream log_file;
    sockbuf client_buf;
    iostream client_ios;
    Codec<iostream> * codec;
    Objects::Encoder * encoder;
    Connection * connection;

  protected:
    virtual void UnknownObjectArrived(const Atlas::Message::Object&);
    virtual void ObjectArrived(const Objects::Operation::Login & op);
    virtual void ObjectArrived(const Objects::Operation::Create & op);
    virtual void ObjectArrived(const Objects::Operation::Move & op);
    virtual void ObjectArrived(const Objects::Operation::Set & op);
    virtual void ObjectArrived(const Objects::Operation::Touch & op);
    virtual void ObjectArrived(const Objects::Operation::Look & op);
    virtual void ObjectArrived(const Objects::Operation::Talk & op);

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

    void send(const Objects::Operation::RootOperation * op) {
        if (op) {
            encoder->StreamMessage(op);
            client_ios << flush;
        }
    }

    int peek() { return client_ios.peek(); }
    int eof() { return client_ios.eof(); }
    int get_fd() { return client_fd; }

    void message(const Objects::Operation::RootOperation &);
    int setup();
};

#endif /* COMM_CLIENT_H */
