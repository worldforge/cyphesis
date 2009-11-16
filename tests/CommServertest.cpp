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

#include "server/CommServer.h"
#include "server/ServerRouting.h"
#include "server/CommSocket.h"

#include "rulesets/Entity.h"

#include "TestWorld.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    Entity e("0", 0);
    TestWorld world(e);

    ServerRouting server(world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    CommServer commServer(server);

    int ret = commServer.setup();
    assert(ret == 0);

    commServer.poll();

    CommFakeSocket * cfs = new CommFakeSocket(commServer);

    assert(ret == 0);

    commServer.addSocket(cfs);

    commServer.poll();

    commServer.removeSocket(cfs);

}
