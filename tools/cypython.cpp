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

/// \page cypython_index
///
/// \section Introduction
///
/// cypython is a commandline tool to inject single python commands into
/// the running server.

#include "common/globals.h"
#include "common/log.h"
#include "common/sockets.h"

#include <skstream/skstream_unix.h>

#include <varconf/config.h>

#ifndef READLINE_CXX_SANE   // defined in config.h
extern "C" {
#endif
#define USE_VARARGS
#define PREFER_STDARG
#include <readline/readline.h>
#include <readline/history.h>
#ifndef READLINE_CXX_SANE
}
#endif

#include <iostream>

int main(int argc, char ** argv)
{
    int config_status = loadConfig(argc, argv, 0); 
    if (config_status < 0) {
        if (config_status == CONFIG_VERSION) {
            reportVersion(argv[0]);
            return 0;
        } else if (config_status == CONFIG_HELP) {
            showUsage(argv[0], USAGE_CYPYTHON, "");
            return 0;
        } else if (config_status != CONFIG_ERROR) {
            log(ERROR, "Unknown error reading configuration.");
        }
        // Fatal error loading config file
        return 1;
    }

    if (config_status > argc) {
        showUsage(argv[0], USAGE_CYPYTHON, "");
        return 1;
    }

    unix_socket_stream sk;

    sk.open(python_socket_name);

    if (!sk.is_open()) {
        std::cerr << "Connection to " << python_socket_name
                  << " failed" << std::endl << std::flush;
        return 1;
    }

    while (!exit_flag) {
        const char * line = readline(">>> ");
        if (line == 0) {
            exit_flag = true;
            std::cout << std::endl << std::flush;
        } else {
            add_history(line);
            sk << line << std::endl << std::flush;
        }
    }

    delete global_conf;
    return 0;
}
