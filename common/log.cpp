// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "log.h"
#include "globals.h"

#include <iostream>

extern "C" {
  #include <syslog.h>
  #include <errno.h>
}

void initLogger()
{
    if (daemon_flag) {
        openlog("WorldForge Cyphesis", LOG_PID, LOG_USER);
    }
}

void log(LogLevel lvl, const char * msg)
{
    if (daemon_flag) {
        int type;
        switch (lvl) {
            case SCRIPT:
            case INFO:
                type = LOG_INFO;
                break;
            case NOTICE:
                type = LOG_NOTICE;
                break;
            case WARNING:
                type = LOG_WARNING;
                break;
            case SCRIPT_ERROR:
            case ERROR:
                type = LOG_ERR;
                break;
            case CRITICAL:
                type = LOG_CRIT;
                break;
            default:
                type = LOG_CRIT;
                break;
        };
        syslog(type, msg);
    } else {
        char * type;
        switch (lvl) {
            case INFO:
                type = "INFO";
                break;
            case SCRIPT:
                type = "SCRIPT";
                break;
            case NOTICE:
                type = "NOTICE";
                break;
            case WARNING:
                type = "WARNING";
                break;
            case ERROR:
                type = "ERROR";
                break;
            case SCRIPT_ERROR:
                type = "SCRIPT_ERROR";
                break;
            case CRITICAL:
                type = "CRITICAL";
                break;
            default:
                type = "UNKNOWN";
                break;
        };
        std::cerr << type << ": " << msg << std::endl << std::flush;
    }
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
