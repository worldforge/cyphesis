// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CLIENT_OBSERVER_CLIENT_H
#define CLIENT_OBSERVER_CLIENT_H

#include "CommClient.h"
#include "ClientConnection.h"

class ObserverClient : public CommClient {
  public:
    ObserverClient();
    bool setup();
    void loadDefault();
    void idle();
};

#endif // CLIENT_OBSERVER_CLIENT_H
