// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef COMMON_LOG_H
#define COMMON_LOG_H

typedef enum log_level { INFO, SCRIPT, NOTICE,
                         WARNING, ERROR, SCRIPT_ERROR, CRITICAL } LogLevel;

typedef enum log_event { START, STOP, CONNECT, DISCONNECT, LOGIN, LOGOUT,
                         TAKE_CHAR, DROP_CHAR } LogEvent;

void initLogger();
void log(LogLevel, const char * msg);
void logEvent(LogEvent, const char * msg);
void logSysError(LogLevel);

#endif // COMMON_LOG_H
