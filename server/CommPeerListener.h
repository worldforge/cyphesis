// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef SERVER_COMM_PEER_LISTENER_H
#define SERVER_COMM_PEER_LISTENER_H

#include "CommListener.h"

/// \brief Handle the internet listen socket used to accept connections from
/// remote clients.
class CommPeerListener : public CommListener {
  private:
    virtual void create(int fd, const char * address);

  public:
    explicit CommPeerListener(CommServer & svr);
    virtual ~CommPeerListener();
};

#endif // SERVER_COMM_PEER_LISTENER_H
