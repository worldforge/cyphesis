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

#ifndef SERVER_JUNCTURE_H
#define SERVER_JUNCTURE_H

#include "ConnectedRouter.h"

#include <sigc++/trackable.h>

class CommPeer;
class Connection;
class Peer;

/// \brief Class managing and persisting connections to another server that
/// is peered to this one
///
/// This is the main point of dispatch for any operation from the manager of
/// the peer connection
class Juncture : public ConnectedRouter, virtual public sigc::trackable {
  protected:
    CommPeer * m_socket;
    Peer * m_peer;

    void onPeerLost();
    void onPeerReplied(const Operation &);
  public:
    Juncture(Connection *, const std::string & id, long iid);
    virtual ~Juncture();

    virtual void operation(const Operation &, OpVector &);

    void LoginOperation(const Operation &, OpVector &);
    void OtherOperation(const Operation &, OpVector &);

    void customConnectOperation(const Operation &, OpVector &);

    int teleportEntity(const Entity *);
};

#endif // SERVER_JUNCTURE_H
