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

#include "HttpCache.h"

#include "common/const.h"
#include "common/globals.h"

#include <varconf/config.h>

#include <iostream>

HttpCache * HttpCache::m_instance = 0;

HttpCache::HttpCache()
{
}

void HttpCache::sendHeaders(std::ostream & io,
                            int status,
                            const std::string & msg)
{
    io << "HTTP/1.1 " << status << " " << msg << std::endl;
    io << "Content-Type: text/html" << std::endl;
    io << "Server: cyphesis/" << consts::version << std::endl << std::endl;
    io << std::flush;
}

void HttpCache::reportBadRequest(std::ostream & io,
                                 int status,
                                 const std::string & msg)
{
    sendHeaders(io, status, msg);
    io << "<html><head><title>" << status << " " << msg
       << "</title></head><body><h1>" << status << " - " << msg
       << "</h1></body></html>" << std::endl << std::flush;
}

void HttpCache::processQuery(std::ostream & io,
                             const std::list<std::string> & headers)
{
    if (headers.empty()) {
        reportBadRequest(io);
        return;
    }
    const std::string & request = headers.front();
    std::string::size_type i = request.find(" ");

    if (i == std::string::npos) {
        reportBadRequest(io);
        return;
    }

    std::string query = request.substr(0, i);
    std::string path;
    ++i;

    std::string::size_type j = request.find(" ", i + 1);

    if (j != std::string::npos) {
        path = request.substr(i, j - i);
    } else {
        path = request.substr(i);
    }

    sendHeaders(io);
    const varconf::sec_map & conf = global_conf->getSection(::instance);

    varconf::sec_map::const_iterator I = conf.begin();
    varconf::sec_map::const_iterator Iend = conf.end();
    for (; I != Iend; ++I) {
        io << I->first << " " << I->second << "<br/>" << std::endl;
    }
    io << std::flush;
}
