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


#ifndef TESTS_COMM_STREAM_LISTENER_STUB_IMPL_H
#define TESTS_COMM_STREAM_LISTENER_STUB_IMPL_H

#include "server/CommStreamListener.h"

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
    return 0;
}

template <class ListenerT>
int CommStreamListener<ListenerT>::getFd() const
{
    return 0;
}

template <class ListenerT>
bool CommStreamListener<ListenerT>::eof()
{
    return false;
}

template <class ListenerT>
bool CommStreamListener<ListenerT>::isOpen() const
{
    return true;
}

template <class ListenerT>
int CommStreamListener<ListenerT>::read()
{
    return 0;
}

template <class ListenerT>
void CommStreamListener<ListenerT>::dispatch()
{
}

template <class ListenerT>
void CommStreamListener<ListenerT>::disconnect()
{
}

template <class ListenerT>
int CommStreamListener<ListenerT>::flush()
{
    return 0;
}

#endif // TESTS_COMM_STREAM_LISTENER_STUB_IMPL_H
