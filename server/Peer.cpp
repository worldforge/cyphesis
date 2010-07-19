// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "Peer.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "CommClient.h"
#include "CommPeer.h"
#include "TeleportState.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::Anonymous;

/// \brief Constructor
///
/// @param client the client socket used to connect to the peer.
/// @param svr the server routing object of this server.
/// @param addr a string representation of the address of the peer.
/// @param id a string giving the indentifier of the peer connection.
Peer::Peer(CommClient & client,
           ServerRouting & svr,
           const std::string & addr,
           const std::string & id) :
      Router(id, forceIntegerId(id)), m_commClient(client), m_server(svr),
      m_state(PEER_INIT)
{
}

Peer::~Peer()
{
}

void Peer::setAuthState(PeerAuthState state)
{
    m_state = state;
}

PeerAuthState Peer::getAuthState()
{
    return m_state;
}

void Peer::operation(const Operation &op, OpVector &res)
{
    const OpNo op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::INFO_NO:
        {
            if (m_state == PEER_AUTHENTICATING) {
                // Response to a Login op
                m_accountId = op->getId();
                if (!op->getParents().empty()) {
                    m_accountType = op->getParents().front();
                }
                if (m_state == PEER_AUTHENTICATING) {
                    m_state = PEER_AUTHENTICATED;
                    log(INFO, "Peer authenticated");
                }
            } else if (m_state == PEER_AUTHENTICATED) {
                // Response to a Create op
                const std::vector<Root> & args = op->getArgs();
                if (args.empty()) {
                    return;
                }
                const Root & arg = args.front();
                if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
                    error(op, "Set arg has no id.", res, getId());
                    return;
                }
                const std::string & id = arg->getId();
                TeleportState *s = getTeleportState(id);
                if(s == NULL) {
                    log(INFO, "Info op for unknown create");
                    return;
                }
                s->setCreated();
                log(INFO, String::compose("Entity with ID %1 created on peer", id));
            }
            break;
        }
    }
}

int Peer::teleportEntity(const RootEntity &entity, Peer &peer)
{
    if (m_state != PEER_AUTHENTICATED) {
        log(ERROR, "Peer not authenticated yet.");
        return -1;
    }
    const std::string &id = entity->getId();
    if (id.empty()) {
        log(ERROR, "Entity has invalid ID");
        return -1;
    }

    Create op;
    op->setFrom(m_accountId);
    op->setArgs1(entity);
    op->setSerialno(newSerialNo());
    m_commClient.send(op);

    TeleportState *s = new TeleportState(id, true);
    if(s == NULL) {
        log(ERROR, "Unable to allocate teleport state object");
        return -1;
    }
    s->setRequested();
    m_teleports.push_back(s);

    return 0;
}

TeleportState *Peer::getTeleportState(const std::string & id)
{
    for (unsigned int i=0;i<m_teleports.size();i++) {
        if (m_teleports[i]->getId() == id)
            return m_teleports[i];
    }
    return NULL;
}
