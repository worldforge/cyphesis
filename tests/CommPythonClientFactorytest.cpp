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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/CommPythonClientFactory.h"
#include "server/CommServer.h"

#include "common/log.h"

#include <cstdio>

#include <cassert>

static bool test_newid_fail = false;

int main()
{
    CommServer comm_server;

    {
        CommPythonClientFactory cpcf;

        int res = cpcf.newCommClient(comm_server, -1, "");
        assert(res == 0);
    }
}

// stubs

#include "CommClient_stub_impl.h"

#include "server/CommPythonClient.h"

#include "common/id.h"

#include <cstdlib>

CommPythonClient::CommPythonClient(CommServer & svr, int fd) :
                  CommStreamClient<tcp_socket_stream>(svr, fd), m_pyContext(0)
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

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

int CommSocket::flush()
{
    return 0;
}

CommClientKit::~CommClientKit()
{
}

CommServer::CommServer() : m_congested(false)
{
}

CommServer::~CommServer()
{
}

int CommServer::addSocket(CommSocket*)
{
    return 0;
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
