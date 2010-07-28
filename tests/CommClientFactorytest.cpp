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

// $Id$

#include "server/CommClientFactory.h"

#include "server/Peer.h"
#include "server/SlaveClientConnection.h"
#include "server/TrustedConnection.h"
#include "server/CommServer.h"

#include "common/log.h"

#include <Atlas/Objects/RootOperation.h>

#include <cstdio>

#include <cassert>

static bool test_newid_fail = false;

class TestCommClientKit : public CommClientKit
{
  public:
    virtual int newCommClient(CommServer &, int, const std::string &) {
        return 0;
    }
};

int main()
{
    CommServer comm_server(*(ServerRouting*)0);

    {
        TestCommClientKit tcck;
    }

    {
        test_newid_fail = false;
        CommClientFactory<Connection> ccf_c;

        int res = ccf_c.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<Connection> ccf_c;

        int res = ccf_c.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }

    {
        test_newid_fail = false;
        CommClientFactory<SlaveClientConnection> ccf_scc;

        int res = ccf_scc.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<SlaveClientConnection> ccf_scc;

        int res = ccf_scc.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }

    {
        test_newid_fail = false;
        CommClientFactory<TrustedConnection> ccf_tc;

        int res = ccf_tc.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<TrustedConnection> ccf_tc;

        int res = ccf_tc.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }

    {
        test_newid_fail = false;
        CommClientFactory<Peer> ccf_p;

        int res = ccf_p.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        test_newid_fail = true;
        CommClientFactory<Peer> ccf_p;

        int res = ccf_p.newCommClient(comm_server, -1, "");
        assert(res != 0);
    }

    {
        CommHttpClientFactory chcf;

        int res = chcf.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }

    {
        CommPythonClientFactory cpcf;

        int res = cpcf.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }
}

// stubs

#include "server/CommClient.h"
#include "server/CommHttpClient.h"
#include "server/CommPythonClient.h"

#include "common/id.h"

#include <cstdlib>

using Atlas::Objects::Root;

Connection::Connection(CommClient & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id) :
            Router(id, 3), m_obsolete(false),
                                                m_commClient(client),
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

TrustedConnection::TrustedConnection(CommClient & client,
                                     ServerRouting & svr,
                                     const std::string & addr,
                                     const std::string & id) :
                                     Connection(client, svr, addr, id)
{
}

Account * TrustedConnection::newAccount(const std::string & type,
                                        const std::string & username,
                                        const std::string & hash,
                                        const std::string & id, long intId)
{
    return 0;
}

SlaveClientConnection::SlaveClientConnection(CommClient & client,
                                             ServerRouting & svr,
                                             const std::string & address,
                                             const std::string & id) :
                       Router(id, -1),
                       m_commClient(client), m_server(svr)
{
}

SlaveClientConnection::~SlaveClientConnection()
{
}

void SlaveClientConnection::operation(const Operation &, OpVector &)
{
}

Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id) :
      Router(id, forceIntegerId(id)), m_commClient(client), m_server(svr)
{
}

Peer::~Peer()
{
}

void Peer::operation(const Operation &, OpVector &)
{
}

int CommServer::addSocket(CommSocket*)
{
    return 0;
}

CommHttpClient::CommHttpClient(CommServer & svr, int fd) :
                CommStreamClient(svr, fd)
{
}

CommHttpClient::~CommHttpClient()
{
}

void CommHttpClient::dispatch()
{
}

int CommHttpClient::read()
{
    return 0;
}

CommPythonClient::CommPythonClient(CommServer & svr, int fd) :
                  CommStreamClient(svr, fd), m_pyContext(0)
{
}

CommPythonClient::~CommPythonClient()
{
}

void CommPythonClient::dispatch()
{
}

int CommPythonClient::read()
{
    return 0;
}

CommClient::CommClient(CommServer & svr, int fd) :
            CommStreamClient(svr, fd), Idle(svr)
{
}

CommClient::~CommClient()
{
}

void CommClient::objectArrived(const Atlas::Objects::Root & obj)
{
}

void CommClient::idle(time_t t)
{
}

int CommClient::read()
{
    return 0;
}

void CommClient::dispatch()
{
}

void CommClient::setup(Router * connection)
{
}

CommStreamClient::CommStreamClient(CommServer & svr, int) :
                  CommSocket(svr)
{
}

CommStreamClient::~CommStreamClient()
{
}

int CommStreamClient::getFd() const
{
    return -1;
}

bool CommStreamClient::isOpen() const
{
    return true;
}

bool CommStreamClient::eof()
{
    return false;
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

CommServer::CommServer(ServerRouting & svr) : m_congested(false), m_server(svr)
{
}

CommServer::~CommServer()
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
