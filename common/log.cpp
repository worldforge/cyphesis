// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#include "log.h"
#include "globals.h"
#include "compose.hpp"

#include <iostream>
#include <fstream>

extern "C" {
  #include <syslog.h>
  #include <errno.h>
}

static void logDate(std::ostream & log_stream)
{
    char buf[256];
    struct tm local_time;

    const time_t now = time(NULL);

    if (localtime_r(&now, &local_time) != &local_time) {
        log_stream << "[TIME_ERROR]";
        return;
    }

    int count = strftime(buf, sizeof(buf) / sizeof(char), "%Y:%m:%d %T", &local_time);

    if (count == 0) {
        log_stream << "[TIME_ERROR]";
        return;
    }

    log_stream << buf;
}

static std::ofstream event_log;

void initLogger()
{
    if (daemon_flag) {
        openlog("WorldForge Cyphesis", LOG_PID, LOG_USER);
    }

    std::string event_log_file = var_directory + "/tmp/cyphesis_event.log";

    event_log.open(event_log_file.c_str(), std::ios::out | std::ios::app);

    if (!event_log.is_open()) {
        log(ERROR, String::compose("Unable to open event log file \"%1\"", event_log_file).c_str());
        logSysError(ERROR);
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
        logDate(std::cerr);
        std::cerr << " " << type << " " << msg << std::endl << std::flush;
    }
}

void logEvent(LogEvent lev, const char * msg)
{
    if (!event_log.is_open()) {
        return;
    }

    char * type;
    switch (lev) {
        case START:
            type = "START";
            break;
        case STOP:
            type = "STOP";
            break;
        case CONNECT:
            type = "CONNECT";
            break;
        case DISCONNECT:
            type = "DISCONNECT";
            break;
        case LOGIN:
            type = "LOGIN";
            break;
        case LOGOUT:
            type = "LOGOUT";
            break;
        case TAKE_CHAR:
            type = "TAKE_CHAR";
            break;
        case DROP_CHAR:
            type = "DROP_CHAR";
            break;
        default:
            type = "UNKNOWN";
            break;
    };

    logDate(event_log);
    event_log << " " << type << " " << msg << std::endl << std::flush;
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
