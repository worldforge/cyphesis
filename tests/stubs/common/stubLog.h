// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2003 Alistair Riddoch
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


#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include "common/log.h"

std::ostream & operator<<(std::ostream & s, LogLevel lvl)
{
    switch (lvl) {
        case INFO:
            s << "INFO";
            break;
        case SCRIPT:
            s << "SCRIPT";
            break;
        case NOTICE:
            s << "NOTICE";
            break;
        case WARNING:
            s << "WARNING";
            break;
        case ERROR:
            s << "ERROR";
            break;
        case SCRIPT_ERROR:
            s << "SCRIPT_ERROR";
            break;
        case CRITICAL:
            s << "CRITICAL";
            break;
        default:
            s << "UNKNOWN";
            break;
    };
    return s;
}

void initLogger()
{}
void rotateLogger()
{}
void log(LogLevel lvl, const std::string & msg)
{
    std::cerr << lvl << " " << msg << std::endl << std::flush;
}
void log_formatted(LogLevel lvl, const std::string & msg)
{
    log(lvl, msg);
}
void logEvent(LogEvent, const std::string & msg)
{
    std::cerr << "(event) " << lvl << " " << msg << std::endl << std::flush;

}
void logSysError(LogLevel lvl)
{
    char * err = strerror(errno);
    if (err != NULL) {
        log(lvl, err);
    } else {
        log(ERROR, "Error getting error message from system.");
    }
}

bool testEventLog(const char * path)
{
    return true;
}

#endif // COMMON_LOG_H
