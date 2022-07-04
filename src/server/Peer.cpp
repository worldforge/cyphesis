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


#include "Peer.h"

#include "ServerRouting.h"
#include "Lobby.h"
#include "TeleportState.h"

#include "rules/simulation/BaseWorld.h"
#include "common/CommSocket.h"
#include "common/log.h"
#include "common/compose.hpp"

#include "rules/LocatedEntity.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <chrono>
#include <rules/simulation/MindsProperty.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Entity::Anonymous;

/// \brief Constructor
///
/// @param client the client socket used to connect to the peer.
/// @param svr the server routing object of this server.
/// @param addr a string representation of the address of the peer.
/// @param id a string giving the indentifier of the peer connection.
Peer::Peer(CommSocket& client,
           ServerRouting& svr,
           const std::string& addr,
           int port,
           RouterId id) :
        Link(client, std::move(id)),
        m_host(addr),
        m_port(port),
        m_state(PEER_INIT),
        m_server(svr)
{
    logEvent(CONNECT, String::compose("%1 - - Connect to %2", id.m_id, addr));
}

Peer::~Peer()
{
    destroyed.emit();
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

void Peer::externalOperation(const Operation& op, Link&)
{
    log(ERROR, String::compose("%1 called", __PRETTY_FUNCTION__));
}

/// \brief Execute an operation sent by a connected peer
///
/// \param op The operation to be executed
/// \param res The result set of replies
void Peer::operation(const Operation& op, OpVector& res)
{
    if (!op->isDefaultRefno()) {
        replied.emit(op);
    }

    auto op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::INFO_NO: {
            // If we receive an Info op while we are not yet authenticated, it
            // can only be the result of an authentication request.
            if (m_state == PEER_AUTHENTICATING) {
                const std::vector<Root>& args = op->getArgs();
                if (args.empty()) {
                    return;
                }
                const Root& arg = args.front();
                if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
                    return;
                }
                // Response to a Login op
                m_accountId = arg->getId();
                m_accountType = arg->getParent();
                m_state = PEER_AUTHENTICATED;
            } else if (m_state == PEER_AUTHENTICATED) {
                // If we received an Info op while authenticated, it is a
                // response to a teleport request.
                peerTeleportResponse(op, res);
            }
        }
            break;
        case Atlas::Objects::Operation::ERROR_NO: {
            m_state = PEER_FAILED;
        }
            break;
        default:
            //Ignore all else
            break;
    }
}

/// \brief Teleport an entity to the connected peer
///
/// @param ent The entity to be teleported
/// @return Returns 0 on success and -1 on failure
int Peer::teleportEntity(const LocatedEntity* ent)
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


    auto teleport_time = std::chrono::steady_clock::now();

    // Add a teleport state object to identify this teleport request
    TeleportState s(teleport_time);

    // Check if the entity has a mind

    Atlas::Objects::Entity::Anonymous atlas_repr;
    ent->addToEntity(atlas_repr);

    Create op;
    op->setFrom(m_accountId);
    op->setSerialno(iid);
    op->setArgs1(atlas_repr);

    auto mindsProperty = ent->getPropertyClassFixed<MindsProperty>();

    if (mindsProperty && !mindsProperty->getMinds().empty()) {
        // Entities with a mind require an additional one-time possess key that
        // is used by the client to authenticate a teleport on the destination
        // peer
        std::string key;
        log(INFO, "Entity has a mind. Generating random key");
        // FIXME non-random, plus potetial timing attack.
        WFMath::MTRand generator;
        for (int i = 0; i < 32; i++) {
            char ch = (char) ((int) 'a' + generator.rand(25));
            key += ch;
        }

        s.setKey(key);
        // Add an additional possess key argument
        log(INFO, String::compose("Adding possess key %1 to Create op", key));
        Anonymous key_arg;
        key_arg->setAttr("possess_key", key);

        std::vector<Root>& create_args = op->modifyArgs();
        create_args.push_back(key_arg);
    }
    this->send(op);
    log(INFO, "Sent Create op to peer");

    // Set it as validated and add to the list of teleports
    s.setRequested();
    m_teleports.emplace(iid, s);
    log(INFO, "Added new teleport state");

    return 0;
}

/// \brief Handle an Info op response sent as reply to a teleport request
///
/// @param op The Info op sent back as reply to a teleport request
/// @param res The result set of replies
void Peer::peerTeleportResponse(const Operation& op, OpVector& res)
{
    log(INFO, "Got a peer teleport response");
    // Response to a Create op
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "Malformed args in Info op");
        return;
    }
    const Root& arg = args.front();

    if (op->isDefaultRefno()) {
        log(ERROR, "Response to teleport has no refno");
        return;
    }

    long iid = op->getRefno();

    auto I = m_teleports.find(iid);
    if (I == m_teleports.end()) {
        log(ERROR, "Info op for unknown create");
        return;
    }

    auto& s = I->second;

    s.setCreated();
    log(INFO, String::compose("Entity with ID %1 replicated on peer", iid));

    // This is the sender entity. This is retreived again rather than
    // relying on a pointer (in the TeleportState object perhaps) as the
    // entity might have been deleted in the time between sending and response
    auto entity = BaseWorld::instance().getEntity(iid);
    if (!entity) {
        log(ERROR, String::compose("No entity found with ID: %1", iid));
        // Clean up the teleport state object
        m_teleports.erase(I);
        return;
    }

    // If entity has a mind, add extra information in the Logout op
    if (s.isMind()) {
        auto mindsProperty = entity->getPropertyClassFixed<MindsProperty>();
        if (mindsProperty->getMinds().empty()) {
            log(ERROR, "No external mind (though teleport state claims it)");
            return;
        }
        std::vector<Root> logout_args;

        Anonymous op_arg;
        op_arg->setId(entity->getId());
        logout_args.push_back(op_arg);

        Anonymous ip_arg;
        ip_arg->setAttr("teleport_host", m_host);
        ip_arg->setAttr("teleport_port", m_port);
        ip_arg->setAttr("possess_key", s.getPossessKey());
        ip_arg->setAttr("possess_entity_id", arg->getId());
        logout_args.push_back(ip_arg);

        Logout logoutOp;
        logoutOp->setArgs(logout_args);
        logoutOp->setTo(entity->getId());
        OpVector temp;

        mindsProperty->sendToMinds(logoutOp, temp);
        log(INFO, "Sent random key to connected mind");
    }

    // FIXME Remove from the world cleanly, not delete.

    // Delete the entity from the current world
    Delete delOp;
    Anonymous del_arg;
    del_arg->setId(entity->getId());
    delOp->setArgs1(del_arg);
    delOp->setTo(entity->getId());
    entity->sendWorld(delOp);
    log(INFO, "Deleted entity from current server");
    logEvent(EXPORT_ENT, String::compose("%1 - %2 Exported entity",
                                         getId(), entity->getId()));

    // Clean up the teleport state object
    m_teleports.erase(I);
}

void Peer::cleanTeleports()
{
    if (m_teleports.empty()) {
        return;
    }
    // Get the current time
    auto curr_time = std::chrono::steady_clock::now();

    for (auto I = m_teleports.begin(); I != m_teleports.end();) {
        auto time_passed = curr_time - I->second.getCreateTime();
        // If 5 seconds have passed, the teleport has failed
        if (std::chrono::duration_cast<std::chrono::seconds>(time_passed).count() >= 10 && I->second.isRequested()) {
            log(INFO, String::compose("Teleport timed out for entity (ID %1)",
                                      I->first));
            I = m_teleports.erase(I);
        } else {
            I = std::next(I);
        }
    }
}
