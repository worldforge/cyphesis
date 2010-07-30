// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/HttpCache.h"

#include "common/globals.h"

#include <varconf/config.h>

#include <cassert>

class TestHttpCache : public HttpCache
{
  public:
    void test_sendHeaders(std::ostream & io,
                          int status,
                          const std::string & type,
                          const std::string & msg) {
        sendHeaders(io, status, type, msg);
    }

    void test_reportBadRequest(std::ostream & io,
                               int status,
                               const std::string & mesg) {
        reportBadRequest(io, status, mesg);
    }
};

int main()
{
    global_conf = varconf::Config::inst();

    {
        HttpCache::instance();
        HttpCache::del();
    }

    // No header, invalid
    {
        HttpCache *hc = HttpCache::instance();

        hc->processQuery(std::cout, std::list<std::string>());

        HttpCache::del();
    }

    // Bad request header
    {
        HttpCache *hc = HttpCache::instance();

        std::list<std::string> headers;
        headers.push_back("boo");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    // Legacy HTTP (0.9??)
    {
        HttpCache *hc = HttpCache::instance();

        std::list<std::string> headers;
        headers.push_back("GET foo");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    // HTTP (n.m??)
    {
        HttpCache *hc = HttpCache::instance();

        std::list<std::string> headers;
        headers.push_back("GET foo HTTP/1.0");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    // HTTP get /config
    {
        HttpCache *hc = HttpCache::instance();

        std::list<std::string> headers;
        headers.push_back("GET /config HTTP/1.0");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    // HTTP get /config with some config
    {
        HttpCache *hc = HttpCache::instance();

        global_conf->setItem(instance, "bar", "value");

        std::list<std::string> headers;
        headers.push_back("GET /config HTTP/1.0");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    // HTTP get /monitors
    {
        HttpCache *hc = HttpCache::instance();

        std::list<std::string> headers;
        headers.push_back("GET /monitors HTTP/1.0");

        hc->processQuery(std::cout, headers);

        HttpCache::del();
    }

    {
        TestHttpCache hc;

        hc.test_sendHeaders(std::cout, 200, "test/html", "OK");
    }

    {
        TestHttpCache hc;

        hc.test_reportBadRequest(std::cout, 200, "Bad request");
    }

    return 0;
}

// stubs

#include "common/Monitors.h"

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{
}

Monitors * Monitors::instance()
{
    if (m_instance == NULL) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::send(std::ostream & io)
{
}

varconf::Config * global_conf = NULL;

std::string instance("test_instance");

namespace consts {

  // Version of the software we are running
  const char * version = "test_version";
}
