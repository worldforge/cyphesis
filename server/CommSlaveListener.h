// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_SLAVE_LISTENER_H
#define SERVER_COMM_SLAVE_LISTENER_H

#include "CommUnixListener.h"

/// \brief Handle the unix listen socket used to accept connections from
/// local clients to admin a slave server.
class CommSlaveListener : public CommUnixListener {
  private:
    virtual void create(int fd);

  public:
    explicit CommSlaveListener(CommServer & svr);
    virtual ~CommSlaveListener();
};

#endif // SERVER_COMM_SLAVE_LISTENER_H
