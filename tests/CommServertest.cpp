// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/CommServer.h"

#include "server/CommSocket.h"

#include "common/log.h"
#include "common/SystemTime.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>

#include <cassert>

class CommFakeSocket : public CommSocket {
  public:
    std::string m_filename;
    int m_fd;

    CommFakeSocket(CommServer & cs) : CommSocket(cs), m_fd(-1) { }

    virtual int getFd() const { return -1; }

    virtual bool isOpen() const { return false; }

    virtual bool eof() {
        return true;
    }

    virtual int read() {
        return -1;
    }

    virtual void dispatch() { }

};

int main(int argc, char ** argv)
{
    CommServer commServer;

    int ret = commServer.setup();
    assert(ret == 0);

    commServer.poll(true);

    CommFakeSocket * cfs = new CommFakeSocket(commServer);

    assert(ret == 0);

    ret = commServer.addSocket(cfs);

    if (ret != 0) {
        commServer.removeSocket(cfs);
    }

    commServer.poll(true);

    commServer.idle(SystemTime(), false);
}

// Stub functions

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

void logSysError(LogLevel lvl)
{
}
