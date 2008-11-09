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

// $Id$

#ifndef COMMON_GLOBALS_H
#define COMMON_GLOBALS_H

#include <string>

namespace varconf {
  class Config;
}

extern const char * CYPHESIS;
extern const char * CLIENT;
extern const char * SLAVE;

extern varconf::Config * global_conf;
extern std::string instance;
extern std::string share_directory;
extern std::string etc_directory;
extern std::string var_directory;
extern std::string client_socket_name;
extern std::string slave_socket_name;
extern std::string ruleset;
extern bool exit_flag;
extern bool daemon_flag;
extern bool restricted_flag;
extern bool database_flag;
extern bool pvp_flag;
extern bool pvp_offl_flag;
extern int timeoffset;
extern int client_port_num;
extern int slave_port_num;
extern int peer_port_num;
extern int http_port_num;
extern int dynamic_port_start;
extern int dynamic_port_end;

static const int CONFIG_OKAY = 0;
static const int CONFIG_ERROR = -1;
static const int CONFIG_HELP = -2;
static const int CONFIG_VERSION = -3;

static const int USAGE_SERVER = 1 << 0;
static const int USAGE_CLIENT = 1 << 1;
static const int USAGE_CYCMD = 1 << 2;
static const int USAGE_DBASE = 1 << 3;

/// Database setup all ok
static const int DATABASE_OKAY = 0;
/// Database connection error
static const int DATABASE_CONERR = -1;
/// Database table creation error
static const int DATABASE_TABERR = -2;

template <typename T>
int readConfigItem(const std::string & section, const std::string & key,
                   T & storage);

int loadConfig(int argc, char ** argv, int usage = USAGE_CLIENT);
void reportVersion(const char * prgname);
void showUsage(const char * prgname, int usage_flags, const char * extras = 0);
void updateUserConfiguration();

#endif // COMMON_GLOBALS_H
