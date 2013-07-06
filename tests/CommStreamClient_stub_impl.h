// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#include "server/CommStreamClient.h"

template <class StreamT>
CommStreamClient<StreamT>::CommStreamClient(CommServer & svr, int fd) :
      CommSocket(svr), m_clientIos(fd)
{
}

template <class StreamT>
CommStreamClient<StreamT>::CommStreamClient(CommServer & svr) :
      CommSocket(svr)
{
}

template <class StreamT>
CommStreamClient<StreamT>::~CommStreamClient()
{
}

template <class StreamT>
int CommStreamClient<StreamT>::getFd() const
{
    return -1;
}

template <class StreamT>
bool CommStreamClient<StreamT>::isOpen() const
{
    return true;
}

template <class StreamT>
bool CommStreamClient<StreamT>::eof()
{
    return false;
}

template <class StreamT>
void CommStreamClient<StreamT>::disconnect()
{
}

template <class StreamT>
int CommStreamClient<StreamT>::flush()
{
    return 0;
}
