// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CLIENT_OBSERVER_CLIENT_H
#define CLIENT_OBSERVER_CLIENT_H

#include "BaseClient.h"
#include "ClientConnection.h"

class ObserverClient : public BaseClient {
  public:
    ObserverClient();
    virtual ~ObserverClient();

    bool setup(const std::string & account, const std::string & password);
    void load(const std::string & package, const std::string & function);
    void idle();
};

#endif // CLIENT_OBSERVER_CLIENT_H
