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
#include "ExternalMind.h"

#include "common/id.h"
#include "common/log.h"
#include "common/system.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include "rulesets/Character.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <wfmath/MersenneTwister.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Logout;
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
      Router(id, forceIntegerId(id)),
      m_state(PEER_INIT),
      m_commClient(client),
      m_server(svr)
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
                const std::vector<Root> & args = op->getArgs();
                if (args.empty()) {
                    return;
                }
                const Root & arg = args.front();
                if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
                    return;
                }
                // Response to a Login op
                m_accountId = arg->getId();
                log(INFO, String::compose("Received account ID: %1", m_accountId));
                if (!op->getParents().empty()) {
                    m_accountType = op->getParents().front();
                }
                if (m_state == PEER_AUTHENTICATING) {
                    m_state = PEER_AUTHENTICATED;
                    log(INFO, "Peer authenticated");
                }
            } else if (m_state == PEER_AUTHENTICATED) {
                peerTeleportResponse(op, res);
            }
            break;
        }
    }
}

int Peer::teleportEntity(const RootEntity &entity)
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
    if (m_teleports.find(id) != m_teleports.end()) {
        log(INFO, "Transfer of this entity already in progress");
        return -1;
    }

    Entity * ent = BaseWorld::instance().getEntity(id);
    if (ent == 0) {
        log(ERROR, String::compose("No entity found with ID: %1", id));
        return -1;
    }
    // Check if the entity has a mind
    bool isMind = true;
    Character * chr = dynamic_cast<Character *>(ent);
    if (!chr) {
        isMind = false;
    }
    if (chr->m_externalMind == 0) {
        isMind = false;
    }
    ExternalMind * mind = 0;
    mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
    if (mind == 0 || !mind->isConnected()) {
        isMind = false;
    }
    std::string key("");
    if (isMind) {
        // Generate a nice and long key
        log(INFO, "Entity has a mind. Generating random key");
        WFMath::MTRand generator;
        for(int i=0;i<32;i++) {
            char ch = (char)((int)'a' + generator.rand(25));
            key += ch;
        }
    }

    TeleportState *s;
    if (isMind) {
        s = new TeleportState(key);
    } else {
        s = new TeleportState();
    }

    if(s == NULL) {
        log(ERROR, "Unable to allocate teleport state object");
        return -1;
    }
    
    if (isMind) {
        // Add an additional possess key argument
        std::vector<Root> create_args;
        Anonymous key_arg;
        key_arg->setAttr("possess_key", key);
        create_args.push_back(entity);
        create_args.push_back(key_arg);

        Create op;
        op->setFrom(m_accountId);
        op->setArgs(create_args);
        op->setSerialno(newSerialNo());
        m_commClient.send(op);
    } else {
        // Plain old create without additional argument
        Create op;
        op->setFrom(m_accountId);
        op->setArgs1(entity);
        op->setSerialno(newSerialNo());
        m_commClient.send(op);
    }
    log(INFO, "Sent Create op to peer");

    s->setRequested();
    m_teleports[id] = s;
    log(INFO, "Added new teleport state");

    return 0;
}

void Peer::peerTeleportResponse(const Operation &op, OpVector &res)
{
    log(INFO, "Got a peer teleport response");
    // Response to a Create op
    const std::vector<Root> & args = op->getArgs();
    if (args.size() != 2) {
        log(ERROR, "Malformed args in Info op");
        return;
    }
    const Root & arg = args.front();
    // We have exactly two arguments;
    const Root & arg2 = args.back();
    if(!arg.isValid() || !arg2.isValid()) {
        log(ERROR, "One or more Info op arguments invalid");
        return;
    }
    // Get the original ID of the entity on this server
    const std::string & id = arg2->getId();

    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        return;
    }
    CommPeer *peer = dynamic_cast<CommPeer*>(&m_commClient);
    if(peer == 0) {
        log(ERROR, "Unable to get CommPeer object");
        return;
    }
    if (m_teleports.find(id) != m_teleports.end()) {
        log(ERROR, "Info op for unknown create");
        return;
    }
    TeleportState *s = m_teleports.find(id)->second;
    s->setCreated();
    log(INFO, String::compose("Entity with ID %1 replicated on peer", id));

    // This is the sender entity. This is retreived again rather than
    // rlying on a pointer (in the TeleportState object perhaps) as the
    // entity might have been deleted in the time between sending and response
    Entity * entity = BaseWorld::instance().getEntity(id);
    if (entity == 0) {
        log(ERROR, String::compose("No entity found with ID: %1", id));
        return;
    }


    // Check if the entity has a mind
    bool isMind = s->isMind();

    // If entity has a mind, add extra information in the Logout op
    if (isMind) {
        Character * chr = dynamic_cast<Character *>(entity);
        if (!chr) {
            log(ERROR, "Entity is not a character");
            return;
        }
        if (chr->m_externalMind == 0) {
            log(ERROR, "No external mind (though teleport state claims it)");
            return;
        }
        ExternalMind * mind = 0;
        mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
        if (mind == 0 || !mind->isConnected()) {
            log(ERROR, "Mind is NULL or not connected");
            return;
        }
        std::vector<Root> logout_args;
        Logout logoutOp;
        Anonymous op_arg;
        op_arg->setId(id);
        logout_args.push_back(op_arg);
        Anonymous ip_arg;
        ip_arg->setAttr("teleport_host", peer->getHost());
        ip_arg->setAttr("teleport_port", peer->getPort());
        ip_arg->setAttr("possess_key", s->getPossessKey());
        ip_arg->setAttr("possess_entity_id", arg->getId());
        logout_args.push_back(ip_arg);
        logoutOp->setArgs(logout_args);
        logoutOp->setTo(id);
        OpVector temp;
        mind->operation(logoutOp, temp);
        log(INFO, "Sent random key to connected mind");
    }

    // Delete the entity from the current world
    Delete delOp;
    Anonymous del_arg;
    del_arg->setId(id);
    delOp->setArgs1(del_arg);
    delOp->setTo(id);
    entity->sendWorld(delOp);
    log(INFO, "Deleted entity from current server");
}
