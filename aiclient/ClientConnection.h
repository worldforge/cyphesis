// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include "common/operations.h"
#include "common/types.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Codec.h>

#include <fstream>

class sockbuf : public filebuf {
  public:
    sockbuf() { }
    sockbuf(int fd) : filebuf(fd) { }
    virtual streampos sys_seek(streamoff, _seek_dir) { return streampos(); }
};

typedef enum client_state {
    INIT,
    CONNECTED,
    LOGGEDIN,
    ERROR
} cstate_t;

class ClientConnection : public Atlas::Objects::Decoder {
  private:
    bool reply_flag;
    bool error_flag;
    cstate_t state;
    int client_fd;
    sockbuf * client_buf;
    std::iostream * ios;
    Atlas::Codec<std::iostream> * codec;
    Atlas::Objects::Encoder * encoder;
    std::string acName;

    dict_t objects;

    void operation(const Atlas::Objects::Operation::RootOperation&);

    virtual void objectArrived(const Atlas::Objects::Operation::Error&);
    virtual void objectArrived(const Atlas::Objects::Operation::Info&);
    virtual void objectArrived(const Atlas::Objects::Operation::Sight&);
    virtual void objectArrived(const Atlas::Objects::Operation::Sound&);
    virtual void objectArrived(const Atlas::Objects::Operation::Touch&);
    virtual void objectArrived(const Atlas::Objects::Operation::Appearance&);
    virtual void objectArrived(const Atlas::Objects::Operation::Disappearance&);

  public:
    ClientConnection();
    ~ClientConnection();

    int read();
    bool connect(const std::string &);
    bool login(const std::string &, const string &);
    bool wait();
    void send(const Atlas::Objects::Root & obj);
    void error(const std::string & message);

    int peek() {
        return ios->peek();
    }
    int eof() {
        return ios->eof();
    }
    int get_fd() {
        return client_fd;
    }

    int setup();
    void loop();
};

#endif // CLIENT_CONNECTION_H
