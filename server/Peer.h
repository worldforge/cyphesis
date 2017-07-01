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


#ifndef SERVER_PEER_H
#define SERVER_PEER_H

#include "common/Link.h"

#include <sigc++/signal.h>

class CommSocket;
class LocatedEntity;
class ServerRouting;
class TeleportState;

enum PeerAuthState {
    PEER_INIT,            /// \brief Peer has just been connected to
    PEER_AUTHENTICATING,  /// \brief Peer is currently authenticating us
    PEER_AUTHENTICATED,   /// \brief We are authenticated and ready on the peer
    PEER_FAILED,          /// \brief We have failed to authenticate
};

typedef std::map<long, TeleportState *> TeleportMap;

/// \brief Class represening connections from another server that is peered to
/// to this one
///
/// This is the main point of dispatch for any operation from the peer.
/// It also handles the state relating to a specific connection, including
/// whether it is authenticated.
class Peer : public Link {
  protected:
    /// \brief Name of host we are connected to
    std::string m_host;
    /// \brief Port number we are connected to
    int m_port;
    /// \brief Account identifier returned after successful login
    std::string m_accountId;
    /// \brief Account type returned after login
    std::string m_accountType;
    /// The authentication state of the peer object
    PeerAuthState m_state;
    /// The states of the various active teleports
    TeleportMap m_teleports;
    
  public:
    /// The server routing object of this server.
    ServerRouting & m_server;

    Peer(CommSocket & client, ServerRouting & svr,
         const std::string & addr, int port, const std::string & id, long iid);
    virtual ~Peer();

    void setAuthState(PeerAuthState state);
    PeerAuthState getAuthState();

    void externalOperation(const Operation & op, Link &) override;
    void operation(const Operation &, OpVector &) override;
    
    int teleportEntity(const LocatedEntity *);
    void peerTeleportResponse(const Operation &op, OpVector &res);

    void cleanTeleports();

    sigc::signal<void> destroyed;
    sigc::signal<void, const Operation &> replied;
};

#endif // SERVER_PEER_H
