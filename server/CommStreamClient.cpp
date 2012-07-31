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

#include "CommStreamClient.h"

#include "common/debug.h"
#include "common/log.h"

#include <skstream/skstream.h>

static const bool debug_flag = true;

template <class StreamT>
CommStreamClient<StreamT>::CommStreamClient(CommServer & svr, int fd) :
                  CommSocket(svr),
                  m_clientIos(fd)
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
    return m_clientIos.getSocket();
}

template <class StreamT>
bool CommStreamClient<StreamT>::isOpen() const
{
    return m_clientIos.is_open();
}

template <class StreamT>
bool CommStreamClient<StreamT>::eof()
{
    return (m_clientIos.fail() ||
            m_clientIos.peek() == StreamT::traits_type::eof());
}

template <class StreamT>
void CommStreamClient<StreamT>::disconnect()
{
    m_clientIos.shutdown();
}

template <class StreamT>
int CommStreamClient<StreamT>::flush()
{
    struct timeval tv = {0, 0};
    fd_set sfds;
    int cfd = m_clientIos.getSocket();
    FD_ZERO(&sfds);
    FD_SET(cfd, &sfds);
    if (select(++cfd, NULL, &sfds, NULL, &tv) > 0) {
        // We only flush to the client if the client is ready
        m_clientIos << std::flush;
    } else {
        // FIXME Establish why this gets hit so much
        debug(std::cout << "Client not ready" << std::endl << std::flush;);
    }
    // This timeout should only occur if the client was really not
    // ready
    if (m_clientIos.timeout()) {
        log(NOTICE, "Client disconnected because of write timeout.");
        m_clientIos.shutdown();
        m_clientIos.setstate(std::iostream::failbit);
        return -1;
    }
    return 0;

}

template class CommStreamClient<tcp_socket_stream>;
