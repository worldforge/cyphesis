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

#ifndef SERVER_COMM_HTTP_CLIENT_H
#define SERVER_COMM_HTTP_CLIENT_H

#include "CommStreamClient.h"

#include <skstream/skstream.h>

#include <list>
#include <string>

/// \brief Handle an internet socket connected to a remote web browser.
/// \ingroup ServerSockets
class CommHttpClient : public CommStreamClient {
  protected:
    std::string m_incoming;
    std::list<std::string> m_headers;
    bool m_req_complete;
  public:
    CommHttpClient(CommServer & svr, int fd);
    virtual ~CommHttpClient();

    virtual int read();
    virtual void dispatch();
};

#endif // SERVER_COMM_HTTP_CLIENT_H
