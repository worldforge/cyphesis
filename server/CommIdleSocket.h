// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_IDLE_SOCKET_H
#define SERVER_COMM_IDLE_SOCKET_H

#include "CommSocket.h"

#include <time.h>

/// \brief Base class for any socket which needs to be polled regularly.
///
/// This could probably be re-implemented as a pure virtual interface which
/// does not inherit from CommSocket.
class CommIdleSocket : public CommSocket {
  public:
    explicit CommIdleSocket(CommServer & svr);
    virtual ~CommIdleSocket();

    virtual void idle(time_t t) = 0;
};

#endif // SERVER_COMM_IDLE_SOCKET_H
