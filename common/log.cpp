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

// $Id: log.cpp,v 1.22 2007-03-19 16:17:08 alriddoch Exp $

#include "log.h"
#include "globals.h"
#include "compose.hpp"

#include <iostream>
#include <fstream>

extern "C" {
#ifdef HAVE_SYSLOG_H
  #include <syslog.h>
#endif // HAVE_SYSLOG_H
  #include <errno.h>
}

#ifndef _WIN32
static const char * TIME_FORMAT = "%F %T";
#else
static const char * TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
#endif

static void logDate(std::ostream & log_stream)
{
    struct tm * local_time;
    const time_t now = time(NULL);

#ifdef HAVE_LOCALTIME_R

    struct tm local_time_buffer;

    local_time = local_time_buffer;

    if (localtime_r(&now, local_time) != &local_time) {
        log_stream << "[TIME_ERROR]";
        return;
    }
#else // HAVE_LOCALTIME_R
    local_time = localtime(&now);
#endif // HAVE_LOCALTIME_R

    char buf[256];
    int count = strftime(buf, sizeof(buf) / sizeof(char), TIME_FORMAT, local_time);

    if (count == 0) {
        log_stream << "[TIME_ERROR]";
        return;
    }

    log_stream << buf;
}

static std::ofstream event_log;

static void open_event_log()
{
    std::string event_log_file = var_directory + "/tmp/cyphesis_event.log";

    event_log.open(event_log_file.c_str(), std::ios::out | std::ios::app);

    if (!event_log.is_open()) {
        log(ERROR, String::compose("Unable to open event log file \"%1\"", event_log_file).c_str());
        logSysError(ERROR);
    }
}

void initLogger()
{
#ifdef HAVE_SYSLOG
    if (daemon_flag) {
        openlog("WorldForge Cyphesis", LOG_PID, LOG_USER);
    }
#endif // HAVE_SYSLOG

    open_event_log();
}

void rotateLogger()
{
    if (event_log.is_open()) {
        event_log.close();
    }

    open_event_log();
}

void log(LogLevel lvl, const char * msg)
{
#ifdef HAVE_SYSLOG
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
#else // HAVE_SYSLOG
    {
#endif // HAVE_SYSLOG

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
