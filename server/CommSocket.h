// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_SOCKET_H
#define SERVER_COMM_SOCKET_H

class CommServer;

class CommSocket {
  public:
    CommServer & commServer;

    CommSocket(CommServer & svr) : commServer(svr) { }

    virtual ~CommSocket();

    virtual int getFd() const = 0;
    virtual bool eof() = 0;
    virtual bool isOpen() const = 0;

    virtual bool read() = 0;
};

#endif // SERVER_COMM_SOCKET_H
