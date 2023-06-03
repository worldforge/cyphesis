// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef SERVER_IDLE_CONNECTOR_H
#define SERVER_IDLE_CONNECTOR_H

#include "common/asio.h"
#include "common/asio.h"
#include "common/asio.h"
#include <boost/asio/steady_timer.hpp>
#include <sigc++/signal.h>

/// \brief Generic class for polling an existing object regularly.
///
/// Emits a sigc signal every time it is called, so can be connected to
/// anything that needs calling.
class IdleConnector  {
  public:
    explicit IdleConnector(boost::asio::io_context & io_context);
    virtual ~IdleConnector();


    sigc::signal<void()> idling;

  protected:
    boost::asio::steady_timer m_timer;

    /// \brief Perform idle tasks once per second.
    void idle();
};

#endif // SERVER_IDLE_CONNECTOR_H
