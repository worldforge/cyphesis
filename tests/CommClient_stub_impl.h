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


#include "server/CommClient.h"

template <class StreamT>
CommClient<StreamT>::CommClient(CommServer & svr,
                                    const std::string & name,
                                    int fd) :
            CommStreamClient<StreamT>(svr, fd), Idle(svr),
            m_codec(NULL), m_encoder(NULL), m_link(NULL)
{
}

template <class StreamT>
CommClient<StreamT>::CommClient(CommServer & svr,
                                const std::string & name) :
            CommStreamClient<StreamT>(svr), Idle(svr),
            m_codec(NULL), m_encoder(NULL), m_link(NULL)
{
}

template <class StreamT>
CommClient<StreamT>::~CommClient()
{
}

template <class StreamT>
void CommClient<StreamT>::setup(Link * connection)
{
}

template <class StreamT>
int CommClient<StreamT>::negotiate()
{
    return 0;
}

template <class StreamT>
int CommClient<StreamT>::operation(const Atlas::Objects::Operation::RootOperation & op)
{
    return 0;
}

template <class StreamT>
void CommClient<StreamT>::dispatch()
{
}

template <class StreamT>
void CommClient<StreamT>::objectArrived(const Atlas::Objects::Root & obj)
{
}

template <class StreamT>
void CommClient<StreamT>::idle(time_t t)
{
}

template <class StreamT>
int CommClient<StreamT>::read()
{
    return 0;
}

template <class StreamT>
int CommClient<StreamT>::send(const Atlas::Objects::Operation::RootOperation & op)
{
    return 0;
}
