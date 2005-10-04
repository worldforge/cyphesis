// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_IDLE_H
#define SERVER_IDLE_H

#include "CommSocket.h"

#include <time.h>

/// \brief Base class for any object which needs to be polled regularly.
///
/// This should be treated as an interface, so can cleanly be used in
/// multiple inheritance.
class Idle {
  protected:
    explicit Idle(CommServer & svr);
  public:
    /// Reference to the object that manages all socket communication.
    CommServer & m_idleManager;

    virtual ~Idle();

    /// \brief Perform idle tasks.
    ///
    /// Called from the server's core idle function whenever it is called.
    virtual void idle(time_t t) = 0;
};

#endif // SERVER_IDLE_H
