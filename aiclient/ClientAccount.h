// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef CLIENT_ACCOUNT_H
#define CLIENT_ACCOUNT_H

#include <common/BaseEntity.h>

class ClientConnection;

class ClientAccount : public BaseEntity {
  private:
    ClientConnection & connection;
  public:
    ClientAccount(const string & name, ClientConnection & con);
    virtual ~ClientAccount() { }

    virtual oplist Operation(const RootOperation & op);
    virtual oplist Operation(const Info & op);
};

#endif // CLIENT_ACCOUNT_H
