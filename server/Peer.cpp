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

#include <sys/time.h>

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
    // FIXME Sometimes we need to be removed from ServerRouting
    // m_server.delObject(this);
}

/// \brief Set the authentication state of the peer connection
///
/// @param state The state to set
void Peer::setAuthState(PeerAuthState state)
{
    m_state = state;
}

/// \brief Get the authentication state of the peer
///
/// \return The current authentication state of the peer
PeerAuthState Peer::getAuthState()
{
    return m_state;
}

/// \brief Execute an operation sent by a connected peer
///
/// \param op The operation to be executed
/// \param res The result set of replies
void Peer::operation(const Operation &op, OpVector &res)
{
    const OpNo op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::INFO_NO:
        {
            // If we receive an Info op while we are not yet authenticated, it
            // can only be the result of an authentication request.
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
                if (!op->getParents().empty()) {
                    m_accountType = op->getParents().front();
                }
                m_state = PEER_AUTHENTICATED;
                log(INFO, "Peer authenticated");
            } else if (m_state == PEER_AUTHENTICATED) {
                // If we received an Info op while authenticated, it is a
                // response to a teleport request.
                peerTeleportResponse(op, res);
            }
        }
        break;
        case Atlas::Objects::Operation::ERROR_NO:
        {
            m_state = PEER_FAILED;
            log(INFO, "Peer login failed");
        }
        break;
    }
}

/// \brief Teleport an entity to the connected peer
///
/// @param entity The entity to be teleported
/// @return Returns 0 on success and -1 on failure
int Peer::teleportEntity(const Entity * ent)
{
    if (m_state != PEER_AUTHENTICATED) {
        log(ERROR, "Peer not authenticated yet.");
        return -1;
    }

    long iid = ent->getIntId();
    if (m_teleports.find(iid) != m_teleports.end()) {
        log(INFO, "Transfer of this entity already in progress");
        return -1;
    }

    // Check if the entity has a mind
    bool isMind = true;
    const Character * chr = dynamic_cast<const Character *>(ent);
    if (!chr || chr->m_externalMind == 0) {
        isMind = false;
    }
    ExternalMind * mind = dynamic_cast<ExternalMind*>(chr->m_externalMind);
    if (mind == 0 || !mind->isConnected()) {
        isMind = false;
    }

    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);
    time_t teleport_time = timeVal.tv_sec;

    // Add a teleport state object to identify this teleport request
    TeleportState * s = new TeleportState(teleport_time);
    if (s == NULL) {
        log(ERROR, "Unable to allocate teleport state object");
        return -1;
    }

    Atlas::Objects::Entity::Anonymous atlas_repr;
    ent->addToEntity(atlas_repr);

    Create op;
    op->setFrom(m_accountId);
    op->setSerialno(iid);
    
    if (isMind) {
        // Entities with a mind require an additional one-time possess key that
        // is used by the client to authenticate a teleport on the destination
        // peer
        std::string key;
        log(INFO, "Entity has a mind. Generating random key");
        // FIXME non-random, plus potetial timing attack.
        WFMath::MTRand generator;
        for(int i=0;i<32;i++) {
            char ch = (char)((int)'a' + generator.rand(25));
            key += ch;
        }

        s->setKey(key);
        // Add an additional possess key argument
        log(INFO, String::compose("Adding possess key %1 to Create op", key));
        std::vector<Root> create_args;
        Anonymous key_arg;
        key_arg->setAttr("possess_key", key);
        create_args.push_back(atlas_repr);
        create_args.push_back(key_arg);

        op->setArgs(create_args);
    } else {
        // Plain old create without additional argument
        op->setArgs1(atlas_repr);
    }
    m_commClient.send(op);
    log(INFO, "Sent Create op to peer");
    
    // Set it as validated and add to the list of teleports
    s->setRequested();
    m_teleports[iid] = s;
    log(INFO, "Added new teleport state");

    return 0;
}

/// \brief Handle an Info op response sent as reply to a teleport request
///
/// @param op The Info op sent back as reply to a teleport request
/// @param res The result set of replies
void Peer::peerTeleportResponse(const Operation &op, OpVector &res)
{
    log(INFO, "Got a peer teleport response");
    // Response to a Create op
    const std::vector<Root> & args = op->getArgs();
    if (args.size() < 1) {
        log(ERROR, "Malformed args in Info op");
        return;
    }
    const Root & arg = args.front();

    if (op->isDefaultRefno()) {
        log(ERROR, "Response to teleport has no refno");
        return;
    }

    long iid = op->getRefno();

    CommPeer *peer = dynamic_cast<CommPeer*>(&m_commClient);
    if(peer == 0) {
        log(ERROR, "Unable to get CommPeer object");
        return;
    }

    TeleportMap::iterator I = m_teleports.find(iid);
    if (I == m_teleports.end()) {
        log(ERROR, "Info op for unknown create");
        return;
    }

    TeleportState *s = I->second;
    assert (s != NULL);

    s->setCreated();
    log(INFO, String::compose("Entity with ID %1 replicated on peer", iid));

    // This is the sender entity. This is retreived again rather than
    // relying on a pointer (in the TeleportState object perhaps) as the
    // entity might have been deleted in the time between sending and response
    Entity * entity = BaseWorld::instance().getEntity(iid);
    if (entity == 0) {
        log(ERROR, String::compose("No entity found with ID: %1", iid));
        // Clean up the teleport state object
        m_teleports.erase(I);
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
        op_arg->setId(String::compose("%1", iid));
        logout_args.push_back(op_arg);
        Anonymous ip_arg;
        ip_arg->setAttr("teleport_host", peer->getHost());
        ip_arg->setAttr("teleport_port", peer->getPort());
        ip_arg->setAttr("possess_key", s->getPossessKey());
        ip_arg->setAttr("possess_entity_id", arg->getId());
        logout_args.push_back(ip_arg);
        logoutOp->setArgs(logout_args);
        logoutOp->setTo(String::compose("%1", iid));
        OpVector temp;
        mind->operation(logoutOp, temp);
        log(INFO, "Sent random key to connected mind");
    }

    // FIXME Remove from the world cleanly, not delete.

    // Delete the entity from the current world
    Delete delOp;
    Anonymous del_arg;
    del_arg->setId(String::compose("%1", iid));
    delOp->setArgs1(del_arg);
    delOp->setTo(String::compose("%1", iid));
    entity->sendWorld(delOp);
    log(INFO, "Deleted entity from current server");

    // Clean up the teleport state object
    m_teleports.erase(I);
}

void Peer::cleanTeleports()
{
    if (m_teleports.size() == 0) {
        return;
    }
    // Get the current time
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);
    time_t curr_time = timeVal.tv_sec;

    TeleportMap::iterator I = m_teleports.begin();
    for(I = m_teleports.begin(); I != m_teleports.end(); ++I) {
        time_t time_passed = curr_time - I->second->getCreateTime();
        // If 5 seconds have passed, the teleport has failed
        if (time_passed >= 10 && I->second->isRequested()) {
            log(INFO, String::compose("Teleport timed out for entity (ID %1)",
                                            I->first));
            // FIXME I think this is unsafe.
            m_teleports.erase(I);
        }
    }
}
