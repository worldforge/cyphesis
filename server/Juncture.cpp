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


#include "Juncture.h"

#include "CommPeer.h"
#include "Connection.h"
#include "Peer.h"
#include "ServerRouting.h"

#include "common/Connect.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/id.h"
#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <boost/asio.hpp>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Login;

using String::compose;

static const bool debug_flag = false;

class PeerAddress {
  public:
//    boost::asio::ip::tcp::resolver::iterator tcp_address a;
    boost::asio::ip::tcp::resolver::iterator i;
};

void Juncture::onSocketConnected()
{
    auto socket = m_socket.lock();
    m_peer = new Peer(*socket, m_connection->m_server,
                      m_host, m_port, getId(), getIntId());

    socket->setup(m_peer);
    m_peer->destroyed.connect(sigc::mem_fun(this, &Juncture::onPeerLost));
    m_peer->replied.connect(sigc::mem_fun(this, &Juncture::onPeerReplied));

    log(INFO, String::compose("Juncture onPeerC succeeded %1", getId()));
    if (m_connection != 0) {
        Anonymous info_arg;
        addToEntity(info_arg);

        Info info;
        info->setArgs1(info_arg);
        if (m_connectRef != 0L) {
            info->setRefno(m_connectRef);
        }

        m_connection->send(info);
    }
    m_socket.reset();
    m_connectRef = 0L;
}

void Juncture::onSocketFailed()
{
    assert(m_address != 0);
    assert(m_peer == 0);
    assert(!m_socket.expired());
    if (m_connection != 0) {
        if (++m_address->i != boost::asio::ip::tcp::resolver::iterator()) {
            if (attemptConnect("foo", 6767) == 0) {
                return;
            }
        }

        Anonymous error_arg;
        error_arg->setAttr("message", "Connection failed");

        Error error;
        error->setArgs1(error_arg);
        if (m_connectRef != 0L) {
            error->setRefno(m_connectRef);
        }
        m_connection->send(error);
    }
    m_socket.reset();
    m_connectRef = 0L;
}

void Juncture::onPeerLost()
{
}

void Juncture::onPeerReplied(const Operation & op)
{
    if (m_connection != 0) {
        m_connection->send(op);
    }
}

int Juncture::attemptConnect(const std::string & hostname, int port)
{
    auto peer = std::make_shared<CommPeer>(m_connection->m_server.getName(),
            m_connection->m_commSocket.m_io_service);
    m_socket = std::weak_ptr<CommPeer>(peer);

    peer->connect(*m_address->i);

    m_host = hostname;
    m_port = port;

    log(INFO, String::compose("Connection in progress %1", getId()));
    peer->connected.connect(sigc::mem_fun(this,
                                              &Juncture::onSocketConnected));
    peer->failed.connect(sigc::mem_fun(this,
                                           &Juncture::onSocketFailed));

    return 0;
}

Juncture::Juncture(Connection * c, const std::string & id, long iid) :
          ConnectableRouter(id, iid, c),
          m_address(0),
          m_socket(),
          m_peer(nullptr),
          m_connectRef(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::externalOperation(const Operation & op, Link &)
{
    log(ERROR, String::compose("%1 called", __PRETTY_FUNCTION__));
    assert(m_connection != 0);
    OpVector reply;
    long serialno = op->getSerialno();
    operation(op, reply);
    OpVector::const_iterator Iend = reply.end();
    for(OpVector::const_iterator I = reply.begin(); I != Iend; ++I) {
        if (!op->isDefaultSerialno()) {
            // Should we respect existing refnos?
            if ((*I)->isDefaultRefno()) {
                (*I)->setRefno(serialno);
            }
        }
        // FIXME detect socket failure here
        m_connection->send(*I);
    }
}

void Juncture::operation(const Operation & op, OpVector & res)
{
    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::LOGIN_NO:
            LoginOperation(op, res);
            break;
        case OP_INVALID:
            break;
        default:
            OtherOperation(op, res);
            break;
    }
}

void Juncture::addToMessage(MapType & omap) const
{
    omap["objtype"] = "obj";
    omap["id"] = getId();
    omap["parents"] = ListType(1, "juncture");
}

void Juncture::addToEntity(const RootEntity & ent) const
{
    ent->setObjtype("obj");
    ent->setId(getId());
    ent->setParents(std::list<std::string>(1,"juncture"));
}

void Juncture::LoginOperation(const Operation & op, OpVector & res)
{
    log(INFO, "Juncture got login");

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "No argument to connect op", res, getId());
        return;
    }
    const Root & arg = args.front();

    Element username_attr;
    if (arg->copyAttr("username", username_attr) != 0 || !username_attr.isString()) {
        error(op, "Argument to connect op has no username", res, getId());
        return;
    }
    const std::string & username = username_attr.String();

    Element password_attr;
    if (arg->copyAttr("password", password_attr) != 0 || !password_attr.isString()) {
        error(op, "Argument to connect op has no password", res, getId());
        return;
    }
    const std::string & password = password_attr.String();

    if (m_peer == 0) {
        error(op, "Juncture not connected", res, getId());
        return;
    }
    assert(m_socket.expired());

    if (m_peer->getAuthState() != PEER_INIT) {
        error(op, "Juncture not ready", res, getId());
        return;
    }

    Anonymous account;
    account->setAttr("username", username);
    account->setAttr("password", password);

    Login l;
    l->setArgs1(account);
    if (!op->isDefaultSerialno()) {
        l->setSerialno(op->getSerialno());
    }
    // Send the login op
    m_peer->send(l);
    m_peer->setAuthState(PEER_AUTHENTICATING);
}

void Juncture::OtherOperation(const Operation & op, OpVector & res)
{
    const int op_type = op->getClassNo();
    if (op_type == Atlas::Objects::Operation::CONNECT_NO) {
        return customConnectOperation(op, res);
    }
}

void Juncture::customConnectOperation(const Operation & op, OpVector & res)
{
    log(INFO, "Juncture got connect");

    if (m_peer != 0) {
        error(op, "Juncture already connected", res, getId());
        return;
    }
    assert(m_socket.expired());

    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        error(op, "No argument to connect op", res, getId());
        return;
    }
    const Root & arg = args.front();
    Element hostname_attr;
    if (arg->copyAttr("hostname", hostname_attr) != 0 ||
        !hostname_attr.isString()) {
        error(op, "Argument to connect op has no hostname", res, getId());
        return;
    }
    const std::string & hostname = hostname_attr.String();

    Element port_attr;
    if (arg->copyAttr("port", port_attr) != 0 || !port_attr.isInt()) {
        error(op, "Argument to connect op has no port", res, getId());
        return;
    }
    int port = port_attr.Int();

    debug(std::cout << "Connecting to " << hostname << std::endl << std::flush;);
    m_address = new PeerAddress;

    boost::asio::ip::tcp::resolver resolver(m_peer->m_commSocket.m_io_service);
    boost::asio::ip::tcp::resolver::query query(hostname, compose("%1", port));

    m_address->i = resolver.resolve(query);
    
//    if (m_address->i == m_address->a.end()) {
//        error(op, "Connect host returned zero addressses", res, getId());
//        return;
//    }

    m_connectRef = op->getSerialno();

    if (attemptConnect(hostname, port) != 0) {
        error(op, "Connection failed", res, getId());
    }
}

int Juncture::teleportEntity(const LocatedEntity * ent)
{
    if (m_peer == 0) {
        log(ERROR, "Attempt to teleport through disconnected juncture");
        return -1;
    }
    return m_peer->teleportEntity(ent);
}
