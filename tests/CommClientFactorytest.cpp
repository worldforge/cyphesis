// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "null_stream.h"

#include "server/CommClientFactory_impl.h"

#include "server/Peer.h"
#include "server/TrustedConnection.h"
#include "server/CommServer.h"

#include "common/log.h"

#include <Atlas/Objects/RootOperation.h>

#include <cstdio>

#include <cassert>

static bool test_newid_fail = false;

class TestCommClient : public CommClient<null_stream> {
  public:
    TestCommClient(CommServer & cs,
                   const std::string & n, 
                   int f) : CommClient<null_stream>(cs, n, f) { }
};

class TestCommClientKit : public CommClientKit
{
  public:
    virtual int newCommClient(CommServer &, int, const std::string &) {
        return 0;
    }
};

int main()
{
    CommServer comm_server;

    {
        TestCommClientKit tcck;
    }

    {
        test_newid_fail = false;
        CommClientFactory<TestCommClient, Connection> ccf_c(*(ServerRouting*)0);

        int res = ccf_c.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<TestCommClient, Connection> ccf_c(*(ServerRouting*)0);

        int res = ccf_c.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }

    {
        test_newid_fail = false;
        CommClientFactory<TestCommClient, TrustedConnection> ccf_tc(*(ServerRouting*)0);

        int res = ccf_tc.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<TestCommClient, TrustedConnection> ccf_tc(*(ServerRouting*)0);

        int res = ccf_tc.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }
}

// stubs

#include "server/CommClient.h"

#include "common/id.h"

#include <cstdlib>

using Atlas::Objects::Root;

Connection::Connection(CommSocket & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Link(client, id, iid), m_obsolete(false),
                                                m_server(svr)
{
}

Connection::~Connection()
{
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & hash,
                                 const std::string & id, long intId)
{
    return 0;
}

int Connection::verifyCredentials(const Account & account,
                                  const Root & creds) const
{
    return 0;
}

void Connection::externalOperation(const Operation & op, Link &)
{
}

void Connection::operation(const Operation & op, OpVector & res)
{
}

void Connection::LoginOperation(const Operation & op, OpVector & res)
{
}

void Connection::CreateOperation(const Operation & op, OpVector & res)
{
}

void Connection::LogoutOperation(const Operation & op, OpVector & res)
{
}

void Connection::GetOperation(const Operation & op, OpVector & res)
{
}

TrustedConnection::TrustedConnection(CommSocket & client,
                                     ServerRouting & svr,
                                     const std::string & addr,
                                     const std::string & id, long iid) :
                                     Connection(client, svr, addr, id, iid)
{
}

TrustedConnection::~TrustedConnection()
{
}

Account * TrustedConnection::newAccount(const std::string & type,
                                        const std::string & username,
                                        const std::string & hash,
                                        const std::string & id, long intId)
{
    return 0;
}

Peer::Peer(CommSocket & client,
           ServerRouting & svr,
           const std::string & addr,
           int port,
           const std::string & id, long iid) :
      Link(client, id, iid), m_server(svr)
{
}

Peer::~Peer()
{
}

void Peer::externalOperation(const Operation & op, Link &)
{
}

void Peer::operation(const Operation &, OpVector &)
{
}

int CommServer::addSocket(CommSocket*)
{
    return 0;
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

Link::Link(CommSocket & socket, const std::string & id, long iid) :
            Router(id, iid), m_encoder(0), m_commSocket(socket)
{
}

Link::~Link()
{
}

void Link::send(const Operation & op) const
{
}

void Link::disconnect()
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

void log(LogLevel lvl, const std::string & msg)
{
}

long forceIntegerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        abort();
    }

    return intId;
}

static long idGenerator = 0;

long newId(std::string & id)
{
    if (test_newid_fail) {
        return -1;
    }
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
}
