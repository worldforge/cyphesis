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


#ifndef SERVER_CONNECTABLE_ROUTER_H
#define SERVER_CONNECTABLE_ROUTER_H

#include "common/Router.h"

class Connection;

/// \brief This is the base class for any entity which has an Atlas
/// compatible indentifier, and can be bound to a connection
///
class ConnectableRouter : public Router {
  protected:
    explicit ConnectableRouter(const std::string & id,
                             long intId,
                             Connection * c = 0);
  public:
    /// \brief The network connection currently subscribed to this object
    Connection * m_connection;

    virtual ~ConnectableRouter();
};

#endif // SERVER_CONNECTABLE_ROUTER_H
