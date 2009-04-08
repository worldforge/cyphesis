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

// $Id$

#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <string>

// Some systems pollute the namespace with defines of ERROR and perhaps
// others.
#undef ERROR

typedef enum log_level { INFO, SCRIPT, NOTICE,
                         WARNING, CYLOG_ERROR, ERROR = CYLOG_ERROR,
                         SCRIPT_ERROR,
                         CRITICAL } LogLevel;

typedef enum log_event { START, STOP, CONNECT, DISCONNECT, LOGIN, LOGOUT,
                         TAKE_CHAR, DROP_CHAR } LogEvent;

void initLogger();
void rotateLogger();
void log(LogLevel, const std::string & msg);
void logEvent(LogEvent, const std::string & msg);
void logSysError(LogLevel);

bool testEventLog(const char * path);

#endif // COMMON_LOG_H
