// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "common/log.h"
#include "common/globals.h"

#include <cassert>

#include <errno.h>

int main()
{
    assert(daemon_flag == false);

    log(INFO, "Info log message.");
    log(SCRIPT, "Script log message.");
    log(NOTICE, "Notice log message.");
    log(WARNING, "Warning log message.");
    log(SCRIPT_ERROR, "Script Error log message.");
    log(CRITICAL, "Critical log message.");

    log_formatted(INFO, "Info message with no \\n");
    log_formatted(INFO, "Info message with a \\n\n");
    log_formatted(INFO, "Info message with a \\n \n and text on another line");
    log_formatted(INFO, "Info message with two \\n\n on two lines\n");

    errno = 0;
    logSysError(INFO);
    logSysError(SCRIPT);
    logSysError(NOTICE);
    logSysError(WARNING);
    logSysError(SCRIPT_ERROR);
    logSysError(CRITICAL);

    errno = -1;
    logSysError(INFO);
    logSysError(SCRIPT);
    logSysError(NOTICE);
    logSysError(WARNING);
    logSysError(SCRIPT_ERROR);
    logSysError(CRITICAL);

    errno = EINVAL;
    logSysError(INFO);
    logSysError(SCRIPT);
    logSysError(NOTICE);
    logSysError(WARNING);
    logSysError(SCRIPT_ERROR);
    logSysError(CRITICAL);

    assert(testEventLog("/dev/null"));

    logEvent(START, "Test start event log message");
    logEvent(STOP, "Test stop event log message");
    logEvent(CONNECT, "Test connect event log message");
    logEvent(DISCONNECT, "Test disconnect event log message");
    logEvent(LOGIN, "Test login event log message");
    logEvent(LOGOUT, "Test logout event log message");
    logEvent(TAKE_CHAR, "Test take character event log message");
    logEvent(DROP_CHAR, "Test drop character event log message");
    logEvent(EXPORT_ENT, "Test export entity event log message");
    logEvent(IMPORT_ENT, "Test import entity event log message");
    logEvent(POSSESS_CHAR, "Test possess character event log message");
    
    return 0;
}

bool daemon_flag = false;
std::string var_directory("/var/tmp");
std::string instance("test_instance");
