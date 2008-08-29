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

// $Id$

#include "CommHttpClient.h"
#include "CommServer.h"
#include "HttpCache.h"

static const bool debug_flag = false;

CommHttpClient::CommHttpClient(CommServer & svr, int fd) : CommSocket(svr),
                                                           m_clientIos(fd),
                                                           m_req_complete(false)
{
    m_clientIos.setTimeout(0,1000); // FIXME?
}

CommHttpClient::~CommHttpClient()
{
    m_clientIos.close();
}

void CommHttpClient::dispatch()
{
    if (!m_req_complete) {
        return;
    }
    HttpCache::instance()->processQuery(m_clientIos, m_headers);
    // m_clientIos << "HTTP/1.1 200 OK" << std::endl << std::flush;
    // m_clientIos << "Content-Type: text/html" << std::endl;
    // m_clientIos << "Server: cyphesis/" << consts::version << std::endl << std::endl;
    // m_clientIos << "<html><head><title>Cyphesis</title></head><body>Cystast</body></html>" << std::endl << std::flush;

    m_clientIos.close();
}

int CommHttpClient::read()
{
    m_clientIos.peek();

    std::streamsize count;

    while ((count = m_clientIos.rdbuf()->in_avail()) > 0) {

        for (int i = 0; i < count; ++i) {

            int next = m_clientIos.rdbuf()->sbumpc();
            if (next == '\n') {
                if (m_incoming.empty()) {
                    m_req_complete = true;
                } else {
                    m_headers.push_back(m_incoming);
                    m_incoming.clear();
                }
            } else if (next == '\r') {
            } else {
                m_incoming.append(1, next);
            }
        }
    }

    // Read from the sockets.

    return 0;
}

int CommHttpClient::getFd() const
{
    return m_clientIos.getSocket();
}

bool CommHttpClient::isOpen() const
{
    return m_clientIos.is_open();
}

bool CommHttpClient::eof()
{
    return (m_clientIos.fail() || m_clientIos.peek() == EOF);
}
