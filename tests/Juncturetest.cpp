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

#include "server/Juncture.h"

#include "common/Connect.h"

#include <Atlas/Objects/Operation.h>

#include <cassert>

int main()
{
    {
        Juncture * j = new Juncture(0, "1", 1);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::RootOperation op;
        j->operation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::Login op;
        j->operation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::RootOperation op;
        j->LoginOperation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::RootOperation op;
        j->OtherOperation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::Connect op;
        j->OtherOperation(op, res);

        delete j;
    }

    {
        Juncture * j = new Juncture(0, "1", 1);

        OpVector res;
        Atlas::Objects::Operation::RootOperation op;
        j->customConnectOperation(op, res);

        delete j;
    }

    return 0;
}

// stubs

#include "server/CommPeer.h"
#include "server/CommServer.h"
#include "server/Connection.h"
#include "server/Peer.h"

#include "common/log.h"

#include <Atlas/Negotiate.h>

namespace Atlas { namespace Objects { namespace Operation {

int CONNECT_NO = 500;

} } }

CommPeer::CommPeer(CommServer & svr, const std::string & username, const std::string & password)
                   : CommClient(svr),
                     m_username(username),
                     m_password(password)
{
}

CommPeer::~CommPeer()
{
}

void CommPeer::idle(time_t t)
{
}

int CommPeer::connect(const std::string & host, int port)
{
    return 0;
}

CommClient::CommClient(CommServer &svr) : CommStreamClient(svr), 
                                        Idle(svr), m_codec(NULL), 
                                        m_encoder(NULL), m_connection(NULL),
                                        m_connectTime(0)
{
    m_negotiate = NULL;
}

CommClient::~CommClient()
{
}

void CommClient::setup(Router * connection)
{
}

void CommClient::objectArrived(const Atlas::Objects::Root & obj)
{
}

void CommClient::idle(time_t t)
{
}

int CommClient::operation(const Atlas::Objects::Operation::RootOperation &op)
{
    return 0;
}

int CommClient::read()
{
    return 0;
}

void CommClient::dispatch()
{
}

int CommClient::negotiate()
{
    delete m_negotiate;
    m_negotiate = NULL;
    return 0;
}

int CommClient::send(const Atlas::Objects::Operation::RootOperation &op)
{
    return 0;
}

CommSocket::CommSocket(CommServer & svr) : m_commServer(svr) { }

CommSocket::~CommSocket()
{
}

CommStreamClient::CommStreamClient(CommServer & svr, int fd) :
                  CommSocket(svr),
                  m_clientIos(fd)
{
}

CommStreamClient::CommStreamClient(CommServer & svr) :
                  CommSocket(svr)
{
}

CommStreamClient::~CommStreamClient()
{
}

int CommStreamClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommStreamClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommStreamClient::eof()
{
    return (m_clientIos.fail() ||
            m_clientIos.peek() == std::iostream::traits_type::eof());
}

Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
}

int CommServer::addSocket(CommSocket*)
{
    return 0;
}

Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id, long iid) :
      Router(id, iid),
      m_state(PEER_INIT),
      m_commClient(client),
      m_server(svr)
{
}

Peer::~Peer()
{
}

PeerAuthState Peer::getAuthState()
{
    return m_state;
}

void Peer::setAuthState(PeerAuthState state)
{
    m_state = state;
}

void Peer::operation(const Operation &op, OpVector &res)
{
}

int Peer::teleportEntity(const Entity * ent)
{
    return 0;
}

void Peer::cleanTeleports()
{
}

Connection::Connection(CommClient & client,
                       ServerRouting & svr,
                       const std::string & addr,
                       const std::string & id, long iid) :
            Router(id, iid), m_obsolete(false),
                                                m_commClient(client),
                                                m_server(svr)
{
}

Account * Connection::newAccount(const std::string & type,
                                 const std::string & username,
                                 const std::string & passwd,
                                 const std::string & id, long intId)
{
    return 0;
}

int Connection::verifyCredentials(const Account &,
                                  const Atlas::Objects::Root &) const
{
    return 0;
}


Connection::~Connection()
{
}


void Connection::operation(const Operation &, OpVector &)
{
}

void Connection::LoginOperation(const Operation &, OpVector &)
{
}

void Connection::LogoutOperation(const Operation &, OpVector &)
{
}

void Connection::CreateOperation(const Operation &, OpVector &)
{
}

void Connection::GetOperation(const Operation &, OpVector &)
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

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

void log(LogLevel lvl, const std::string & msg)
{
}
