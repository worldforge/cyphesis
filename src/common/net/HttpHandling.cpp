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


#include "HttpHandling.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/Monitors.h"
#include "common/log.h"

#include <varconf/config.h>
#include <fstream>

HttpHandling::HttpHandling(const Monitors& monitors)
        : m_monitors(monitors) {

    //Should we perhaps make this something the caller has to register?
    mHandlers.emplace_back([this](HttpHandleContext
                                  context) -> HandleResult {
        if (context.path == "/config") {
            sendHeaders(context.io);
            auto& conf = global_conf->getSection(::instance);

            for (auto& entry: conf) {
                context.io << entry.first << " " << entry.second << "\n";
            }
            return HandleResult::Handled;
        } else if (context.path == "/monitors") {
            sendHeaders(context.io);
            m_monitors.send(context.io);
            return HandleResult::Handled;
        } else if (context.path == "/monitors/numerics") {
            sendHeaders(context.io);
            m_monitors.sendNumerics(context.io);
            return HandleResult::Handled;
        } else {
            return HandleResult::Ignored;
        }
    });
}

void HttpHandling::sendHeaders(std::ostream& io,
                               int status,
                               const std::string& type,
                               const std::string& msg,
                               std::vector<std::string> extraHeaders) {
    io << "HTTP/1.1 " << status << " " << msg << "\n";
    io << "Content-Type: " << type << "\n";
    io << "Server: cyphesis/" << consts::version << "\n";
    for (auto& header: extraHeaders) {
        io << header << "\n";
    }
    io << "\n";
}

void HttpHandling::reportBadRequest(std::ostream& io,
                                    int status,
                                    const std::string& msg) {
    sendHeaders(io, status, "text/html", msg);
    io << "<html><head><title>" << status << " " << msg
       << "</title></head><body><h1>" << status << " - " << msg
       << "</h1></body></html>\n";
}

void HttpHandling::processQuery(std::ostream& io,
                                const std::list<std::string>& headers) {
    if (headers.empty()) {
        reportBadRequest(io);
        return;
    }
    const std::string& request = headers.front();
    std::string::size_type i = request.find(" ");

    if (i == std::string::npos) {
        reportBadRequest(io);
        return;
    }

    std::string path;
    ++i;

    std::string::size_type j = request.find(" ", i + 1);

    if (j != std::string::npos) {
        path = request.substr(i, j - i);
    } else {
        path = request.substr(i);
    }

    for (auto& handler: mHandlers) {
        auto result = handler({io, headers, path});
        if (result == HandleResult::Handled) {
            return;
        }
    }
    log(NOTICE, std::string("Path '") + path + "'not found.");
    reportBadRequest(io, 404, "Not Found");
}
