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


#include "IdleConnector.h"
#include "Remotery.h"

/// \brief Constructor for socket object.
///
IdleConnector::IdleConnector(boost::asio::io_context & io_context) :  m_timer(io_context)
{
    idle();
}

IdleConnector::~IdleConnector() = default;

void IdleConnector::idle()
{
#if BOOST_VERSION >= 106600
    m_timer.expires_after(std::chrono::seconds(1));
#else
    m_timer.expires_from_now(std::chrono::seconds(1));
#endif
    m_timer.async_wait([this](boost::system::error_code ec){
        if (!ec) {
            rmt_ScopedCPUSample(idling, 0)
            this->idling.emit();
            this->idle();
        }
    });
}
