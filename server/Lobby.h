// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include "Account.h"

class ServerRouting;

/// \brief This class handles the default out-of-game chat area that all
/// Account objects that are currently logged in are subscribed to by default.
///
/// This allows chat between all connected players, and provides a forum for
/// discusion out of the context of the game.
class Lobby : public OOGThing {
  private:
    AccountDict m_accounts;
    ServerRouting & m_server;
  public:
    explicit Lobby(const std::string & id, ServerRouting &);

    void addObject(Account * a);
    void delObject(Account * a);

    inline const AccountDict & getAccounts() const {
        return m_accounts;
    }

    virtual void operation(const Operation &, OpVector &);

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;
};

#endif // SERVER_LOBBY_H
