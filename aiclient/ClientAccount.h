// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_ACCOUNT_H
#define CLIENT_ACCOUNT_H

#include <common/BaseEntity.h>

class ClientConnection;

class ClientAccount : public BaseEntity {
  private:
    ClientConnection & connection;
  public:
    ClientAccount(const std::string & name, ClientConnection & con);
    virtual ~ClientAccount() { }

    virtual oplist OtherOperation(const RootOperation & op);
    virtual oplist InfoOperation(const Info & op);
};

#endif // CLIENT_ACCOUNT_H
