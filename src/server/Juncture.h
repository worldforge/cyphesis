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


#ifndef SERVER_JUNCTURE_H
#define SERVER_JUNCTURE_H

#include "ConnectableRouter.h"

#include <sigc++/trackable.h>

#include <memory>
#include <boost/asio/ip/tcp.hpp>

class CommPeer;
class Connection;
class LocatedEntity;
class Peer;

struct PeerAddress {
    boost::asio::ip::tcp::resolver::iterator i;
};

/// \brief Class managing and persisting connections to another server that
/// is peered to this one
///
/// This is the main point of dispatch for any operation from the manager of
/// the peer connection. It represents the persistent part of a relationship
/// with another server. Network session specific state is handled by
/// the Peer object, which this object holds a reference to, but does not
/// own.
class Juncture : public ConnectableRouter, virtual public sigc::trackable {
  protected:

    /// \brief The network connection currently subscribed to this object
    Connection * m_connection;
    PeerAddress m_address;

    std::weak_ptr<CommPeer> m_socket;
    Peer * m_peer;
    long m_connectRef;
    std::string m_host;
    int m_port;

    void onSocketConnected();
    void onSocketFailed();
    void onPeerLost();
    void onPeerReplied(const Operation &);

    int attemptConnect(const std::string &, int);
  public:
    Juncture(Connection *, RouterId id);

    ~Juncture() override;

    void externalOperation(const Operation & op, Link &) override;

    void operation(const Operation &, OpVector &) override;

    void addToMessage(Atlas::Message::MapType &) const override;

    void addToEntity(const Atlas::Objects::Entity::RootEntity &) const override;

    void LoginOperation(const Operation &, OpVector &);
    void OtherOperation(const Operation &, OpVector &);

    void customConnectOperation(const Operation &, OpVector &);

    int teleportEntity(const LocatedEntity *);

    void setConnection(Connection* connection) override;

    Connection* getConnection() const override;
};

#endif // SERVER_JUNCTURE_H
