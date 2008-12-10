// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include "Account.h"

class ServerRouting;

/// \brief This class handles the default out-of-game chat area that all
/// Account objects that are currently logged in are subscribed to by default.
///
/// This allows chat between all connected players, and provides a forum for
/// discusion out of the context of the game.
class Lobby : public Router {
  private:
    AccountDict m_accounts;
    ServerRouting & m_server;
  public:
    explicit Lobby(ServerRouting &, const std::string & id, long intId);
    virtual ~Lobby();

    void addAccount(Account * a);
    void delAccount(Account * a);

    inline const AccountDict & getAccounts() const {
        return m_accounts;
    }

    virtual void operation(const Operation &, OpVector &);

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;
};

#endif // SERVER_LOBBY_H
