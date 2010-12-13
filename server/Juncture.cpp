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

#include "Juncture.h"

#include "CommPeer.h"
#include "CommServer.h"
#include "Connection.h"
#include "Peer.h"

#include "common/Connect.h"

#include "common/compose.hpp"
#include "common/debug.h"
#include "common/id.h"
#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Login;

static const bool debug_flag = false;

void Juncture::onPeerLost()
{
#if 0
    // FIXME m_connection doesn't get zeroed yet
    if (m_connection != 0) {
        Anonymous error_arg;
        error_arg->setAttr("message", "Connection lost");

        Error error;
        error->setArgs1(error_arg);
        if (m_socket->getRef() != 0L) {
            error->setRefno(m_socket->getRef());
            m_connection->m_commClient.send(error);
        }
    }
#endif
    m_peer = 0;
    m_socket = 0;
}

void Juncture::onPeerReplied(const Operation & op)
{
    if (m_connection != 0) {
        m_connection->m_commClient.send(op);
    }
}

Juncture::Juncture(Connection * c,
                   const std::string & id, long iid) :
          ConnectedRouter(id, iid, c),
                                                       m_socket(0),
                                                       m_peer(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
    const OpNo op_no = op->getClassNo();
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
    assert(m_socket != 0);

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
    m_peer->m_commClient.send(l);
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
    assert(m_socket == 0);

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

    m_socket = new CommPeer(m_connection->m_commClient.m_commServer);

    debug(std::cout << "Connecting to " << hostname << std::endl << std::flush;);
    if (m_socket->connect(hostname, port, op->getSerialno()) != 0) {
        error(op, "Connection failed", res, getId());
        delete m_socket;
        m_socket = 0;
        return;
    }
    log(INFO, String::compose("Connection succeeded %1", getId()));
    m_peer = new Peer(*m_socket, m_connection->m_server,
                      hostname, getId(), getIntId());
    m_socket->setup(m_peer);
    m_connection->m_commClient.m_commServer.addSocket(m_socket);
    m_connection->m_commClient.m_commServer.addIdle(m_socket);

    m_peer->destroyed.connect(sigc::mem_fun(this, &Juncture::onPeerLost));
    m_peer->replied.connect(sigc::mem_fun(this, &Juncture::onPeerReplied));
}

int Juncture::teleportEntity(const Entity * ent)
{
    if (m_peer == 0) {
        log(ERROR, "Attempt to teleport through disconnected juncture");
        return -1;
    }
    return m_peer->teleportEntity(ent);
}
