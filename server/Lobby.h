// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_LOBBY_H
#define SERVER_LOBBY_H

#include "Account.h"

class ServerRouting;

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

    virtual OpVector operation(const RootOperation & op);

    virtual void addToObject(Atlas::Message::Object::MapType &) const;
};

#endif // SERVER_LOBBY_H
