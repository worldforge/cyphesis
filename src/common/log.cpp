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


#ifdef HAVE_CONFIG_H

#include "config.h"

#endif

#include "log.h"
#include "globals.h"
#include "compose.hpp"

#include <iostream>
#include <fstream>

#include <cstring>
#include <ctime>
#include <functional>

extern "C" {
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif // HAVE_SYSLOG_H
}

static void logDate(std::ostream& log_stream)
{
    auto t = std::time(nullptr);
    char buf[32];
    if (std::strftime(buf, sizeof(buf), "%FT%T", std::localtime(&t))) {
        log_stream << buf;
    } else {
        log_stream << "[TIME_ERROR]";
    }
}

static std::ofstream event_log;

static void open_event_log()
{
    std::string event_log_file = var_directory + "/tmp/cyphesis_event.log";

    event_log.open(event_log_file.c_str(), std::ios::out | std::ios::app);

    if (!event_log.is_open()) {
        log(ERROR, String::compose("Unable to open event log file \"%1\"",
                                   event_log_file));
        logSysError(ERROR);
    }
}

bool testEventLog(const char* path)
{
    event_log.open(path, std::ios::out);
    return event_log.is_open();
}

namespace {
    std::string logging_prefix;
}

void setLoggingPrefix(std::string prefix)
{
    logging_prefix = std::move(prefix);
}


void initLogger()
{
#ifdef HAVE_SYSLOG
    std::string ident = String::compose("Cyphesis{%1}", instance);
    if (daemon_flag) {
        openlog(strdup(ident.c_str()), LOG_PID, LOG_DAEMON);
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

std::function<std::string()> s_logPrefixFn;

std::ostream& operator<<(std::ostream& s, LogLevel lvl);

std::ostream& operator<<(std::ostream& s, LogLevel lvl)
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
    }
    return s;
}

void log(LogLevel lvl, const std::string& msg)
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
        }
        if (!logging_prefix.empty()) {
            syslog(type, "%s %s", logging_prefix.c_str(), msg.c_str());
        } else {
            syslog(type, "%s", msg.c_str());
        }

    } else {
#else // HAVE_SYSLOG
        {
#endif // HAVE_SYSLOG

        logDate(std::cout);
        if (!logging_prefix.empty()) {
            std::cout << " " << logging_prefix;
        }

        if (s_logPrefixFn) {
            std::cout << " " << lvl << " " << s_logPrefixFn() << msg << std::endl;
        } else {
            std::cout << " " << lvl << " " << msg << std::endl;
        }

    }
}

void log_formatted(LogLevel lvl, const std::string& msg)
{
    std::string::size_type s = 0;
    std::string::size_type p = msg.find('\n');
    do {
        log(lvl, msg.substr(s, p - s));
        s = p + 1;
        p = msg.find('\n', s);
    } while (p != std::string::npos);

    // If the last line is not terminated with a newline, print it.
    if (s < msg.size()) {
        log(lvl, msg.substr(s));
    }
}

void logEvent(LogEvent lev, const std::string& msg)
{
    if (!event_log.is_open()) {
        return;
    }

    const char* type;
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
        case EXPORT_ENT:
            type = "EXPORT_ENT";
            break;
        case IMPORT_ENT:
            type = "IMPORT_ENT";
            break;
        case POSSESS_CHAR:
            type = "POSSESS_CHAR";
            break;
        default:
            type = "UNKNOWN";
            break;
    }

    logDate(event_log);
    event_log << " " << instance << " " << type << " " << msg
              << std::endl;
}

void logSysError(LogLevel lvl)
{
    char* err = strerror(errno);
    if (err != nullptr) {
        log(lvl, err);
    } else {
        log(ERROR, "Error getting error message from system.");
    }
}
