// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifndef SERVER_COMM_STREAM_LISTENER_IMPL_H
#define SERVER_COMM_STREAM_LISTENER_IMPL_H

#include "CommStreamListener.h"

#include "CommClientFactory.h"

/// \brief Constructor for stream listener socket object.
///
/// @param svr Reference to the object that manages all socket communication.
template <class ListenerT>
CommStreamListener<ListenerT>::CommStreamListener(CommServer & svr,
      const boost::shared_ptr<CommClientKit> & kit) :
      CommSocket(svr), m_clientKit(kit)
{
}

template <class ListenerT>
CommStreamListener<ListenerT>::~CommStreamListener()
{
}

template <class ListenerT>
int CommStreamListener<ListenerT>::create(int asockfd, const char * address)
{
    return m_clientKit->newCommClient(m_commServer, asockfd, address);
}

template <class ListenerT>
int CommStreamListener<ListenerT>::getFd() const
{
    return m_listener.getSocket();
}

template <class ListenerT>
bool CommStreamListener<ListenerT>::eof()
{
    return false;
}

template <class ListenerT>
bool CommStreamListener<ListenerT>::isOpen() const
{
    return m_listener.is_open();
}

template <class ListenerT>
int CommStreamListener<ListenerT>::read()
{
    this->accept();
    // Accept errors are not returned, as the listen socket should not
    // be removed.
    return 0;
}

template <class ListenerT>
void CommStreamListener<ListenerT>::dispatch()
{
}

#endif // SERVER_COMM_STREAM_LISTENER_IMPL_H
