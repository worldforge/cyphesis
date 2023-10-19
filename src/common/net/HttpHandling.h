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


#ifndef SERVER_HTTP_HANDLING_H
#define SERVER_HTTP_HANDLING_H

#include <list>
#include <string>
#include <filesystem>
#include "common/Monitors.h"
#include "CommHttpClient.h"

struct HttpHandleContext {
    std::ostream& io;
    const std::list<std::string>& headers;
    const std::string& path;
};


/// \brief A caching generator for the results of http requests.
///
class HttpHandling : public HttpRequestProcessor {
public:
    enum class HandleResult {
        Handled, Ignored
    };
    typedef std::function<HandleResult(HttpHandleContext)> HttpHandler;


    HttpHandling(const Monitors& monitors);

    void processQuery(std::ostream&, const std::list<std::string>&);

    //std::vector<std::unique_ptr<HttpPathHandler>> mHandlers;
    std::vector<HttpHandler> mHandlers;

    static void sendHeaders(std::ostream&,
                            int status = 200,
                            const std::string& type = "text/plain",
                            const std::string& mesg = "OK",
                            std::vector<std::string> extraHeaders = {});

    static void reportBadRequest(std::ostream&,
                                 int status = 400,
                                 const std::string& mesg = "Bad Request");

private:
    const Monitors& m_monitors;

};

#endif // SERVER_HTTP_HANDLING_H
