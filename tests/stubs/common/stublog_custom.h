//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include "string.h"
#include <iostream>

std::function<std::string()> s_logPrefixFn;

inline std::ostream & operator<<(std::ostream & s, LogLevel lvl)
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
void logEvent(LogEvent lvl, const std::string & msg)
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
