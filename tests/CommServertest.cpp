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

#include "server/CommSocket.h"
#include "server/ServerRouting.h"

#include "common/BaseWorld.h"
#include "common/log.h"

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

class TestWorld : public BaseWorld {
  public:
    explicit TestWorld() : BaseWorld(*(Entity*)0) {
    }

    virtual bool idle(int, int) { return false; }
    virtual Entity * addEntity(Entity * ent) { 
        return 0;
    }
    virtual Entity * addNewEntity(const std::string &,
                                  const Atlas::Objects::Entity::RootEntity &) {
        return 0;
    }
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         Entity *) { return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    Entity * spawnNewEntity(const std::string & name,
                            const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual Task * newTask(const std::string &, Character &) { return 0; }
    virtual Task * activateTask(const std::string &, const std::string &,
                                const std::string &, Character &) { return 0; }
    virtual void message(const Atlas::Objects::Operation::RootOperation & op,
                         Entity & ent) { }
    virtual ArithmeticScript * newArithmetic(const std::string &, Entity *) {
        return 0;
    }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(Entity *) { }
};

int main(int argc, char ** argv)
{
    TestWorld world;

    ServerRouting server(world, "noruleset", "unittesting",
                         "1", 1, "2", 2);

    CommServer commServer(server);

    int ret = commServer.setup();
    assert(ret == 0);

    commServer.poll();

    CommFakeSocket * cfs = new CommFakeSocket(commServer);

    assert(ret == 0);

    ret = commServer.addSocket(cfs);

    if (ret != 0) {
        commServer.removeSocket(cfs);
    }

    commServer.poll();

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

ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{
}

ServerRouting::~ServerRouting()
{
}

void ServerRouting::addToMessage(Atlas::Message::MapType & omap) const
{
}

void ServerRouting::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}
