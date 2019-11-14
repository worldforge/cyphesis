// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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


#ifndef COMMON_GLOBALS_H
#define COMMON_GLOBALS_H

#include <string>

namespace varconf {
  class Config;
}

extern const char * const CYPHESIS;
extern const char * const SLAVE;

extern varconf::Config * global_conf;
extern std::string instance;
extern std::string bin_directory;
extern std::string share_directory;
extern std::string etc_directory;
extern std::string var_directory;
extern std::string assets_directory;
extern std::string ruleset_name;
extern std::string server_uuid;
extern std::string server_key;
/// @brief Controls "hard" shutdowns
///
/// When this is set to true, the server will exit. It will try to
/// persist entity data to disk, but won't try to get any data from
/// external clients (in contrast to "exit_flag_soft").
extern bool exit_flag;
/// @brief Controls "soft" shutdowns
///
/// When this is set to true, the server will try to perform a "soft"
/// shutdown where it first spends a couple of seconds trying to get
/// data for persistence from external clients. Once that's done,
/// the exit_flag will be set to true, and the server will exit.
extern bool exit_flag_soft;
/// @brief Controls whether "soft" shutdowns are enabled.
///
/// When this is set to true, the server will try to perform a "soft"
/// shutdown when certain signals are received.
/// This is off by default.
extern bool exit_soft_enabled;
extern bool daemon_flag;
extern int timeoffset;
extern int dynamic_port_start;
extern int dynamic_port_end;

static const int CONFIG_OKAY = 0;
static const int CONFIG_ERROR = -1;
static const int CONFIG_HELP = -2;
static const int CONFIG_VERSION = -3;

static const unsigned int USAGE_SERVER = 1u << 0u;
static const unsigned int USAGE_CLIENT = 1u << 1u;
static const unsigned int USAGE_CYCMD = 1u << 2u;
static const unsigned int USAGE_DBASE = 1u << 3u;
static const unsigned int USAGE_CYPYTHON = 1u << 4u;
static const unsigned int USAGE_AICLIENT = 1u << 5u;

static const unsigned int SPM = 20; // seconds per minute
static const unsigned int MPH = 60; // minutes per hour
static const unsigned int HPD = 24; // hours per day
static const unsigned int DPM = 28; // days per month
static const unsigned int MPY = 12; // months per year

/// Database setup all ok
static const int DATABASE_OKAY = 0;
/// Database connection error
static const int DATABASE_CONERR = -1;
/// Database table creation error
static const int DATABASE_TABERR = -2;

#define INT_OPTION(_var, _val, _section, _setting, _help) \
int _var = _val; \
int_config_register _var ## _register(_var, _section, _setting, _help);

#define BOOL_OPTION(_var, _val, _section, _setting, _help) \
bool _var = _val; \
bool_config_register _var ## _register(_var, _section, _setting, _help);

#define STRING_OPTION(_var, _val, _section, _setting, _help) \
std::string _var = _val; \
string_config_register _var ## _register(_var, _section, _setting, _help);

#define UNIXSOCK_OPTION(_var, _val, _section, _setting, _help, _format) \
std::string _var = _val; \
unixsock_config_register _var ## _register(_var, _section, _setting, _help, _format);

/// \brief Object to register an integer varconf option on construction
class int_config_register {
  public:
    int_config_register(int &, const char *, const char *, const char *);
};

/// \brief Object to register a bool varconf option on construction
class bool_config_register {
  public:
    bool_config_register(bool &, const char *, const char *, const char *);
};

/// \brief Object to register a string varconf option on construction
class string_config_register {
  public:
    string_config_register(std::string &, const char *, const char *, const char *);
};

/// \brief Object to register a socket varconf option on construction
class unixsock_config_register {
  public:
    unixsock_config_register(std::string &, const char *, const char *, const char *, const char *);
};

template <typename T>
int readConfigItem(const std::string & section, const std::string & key,
                   T & storage);

int loadConfig(int argc, char ** argv, int usage = USAGE_CLIENT);
void reportVersion(const char * prgname);
void showUsage(const char * prgname, unsigned int usage_flags, const char * extras = nullptr);
void updateUserConfiguration();

#endif // COMMON_GLOBALS_H
