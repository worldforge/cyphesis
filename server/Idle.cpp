// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Idle.h"

#include "CommServer.h"

/// \brief Constructor for socket object.
///
/// @param svr Reference to the object that manages all socket communication.
Idle::Idle(CommServer & svr) : m_idleManager(svr)
{
}

Idle::~Idle()
{
    m_idleManager.removeIdle(this);
}
