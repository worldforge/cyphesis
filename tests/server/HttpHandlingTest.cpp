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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/net/HttpHandling.h"

#include "common/globals.h"

#include <varconf/config.h>

#include <cassert>

class TestHttpCache : public HttpHandling
{
  public:
        TestHttpCache(const Monitors& m): HttpHandling(m)
        { }

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
        HttpHandling httpCache(Monitors::instance());
    }

    // No header, invalid
    {
        HttpHandling hc(Monitors::instance());

        hc.processQuery(std::cout, std::list<std::string>());

    }

    // Bad request header
    {
        HttpHandling hc(Monitors::instance());

        std::list<std::string> headers;
        headers.push_back("boo");

        hc.processQuery(std::cout, headers);

    }

    // Legacy HTTP (0.9??)
    {
        HttpHandling hc(Monitors::instance());

        std::list<std::string> headers;
        headers.push_back("GET foo");

        hc.processQuery(std::cout, headers);

    }

    // HTTP (n.m??)
    {
        HttpHandling hc(Monitors::instance());

        std::list<std::string> headers;
        headers.push_back("GET foo HTTP/1.0");

        hc.processQuery(std::cout, headers);

    }

    // HTTP get /config
    {
        HttpHandling hc(Monitors::instance());

        std::list<std::string> headers;
        headers.push_back("GET /config HTTP/1.0");

        hc.processQuery(std::cout, headers);

    }

    // HTTP get /config with some config
    {
        HttpHandling hc(Monitors::instance());

        global_conf->setItem(instance, "bar", "value");

        std::list<std::string> headers;
        headers.push_back("GET /config HTTP/1.0");

        hc.processQuery(std::cout, headers);

    }

    // HTTP get /monitors
    {
        HttpHandling hc(Monitors::instance());

        std::list<std::string> headers;
        headers.push_back("GET /monitors HTTP/1.0");

        hc.processQuery(std::cout, headers);

    }

    {
        TestHttpCache hc(Monitors::instance());

        hc.test_sendHeaders(std::cout, 200, "test/html", "OK");
    }

    {
        TestHttpCache hc(Monitors::instance());

        hc.test_reportBadRequest(std::cout, 200, "Bad request");
    }

    return 0;
}

// stubs

#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stublog.h"


varconf::Config * global_conf = nullptr;

std::string instance("test_instance");

namespace consts {

  // Version of the software we are running
  const char * version = "test_version";
}
