// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_SLAVE_CLIENT_H
#define SERVER_COMM_SLAVE_CLIENT_H

#include "CommClient.h"

/// \brief Handle an internet socket connected to a local client for a slave
/// server.
class CommSlaveClient : public CommClient {
  public:
    CommSlaveClient(CommServer & svr, int fd);
    virtual ~CommSlaveClient();

};

#endif // SERVER_COMM_SLAVE_CLIENT_H
