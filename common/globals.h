// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001-2004 Alistair Riddoch

#ifndef COMMON_GLOBALS_H
#define COMMON_GLOBALS_H

#include <string>
#include <vector>

namespace varconf {
  class Config;
}

extern varconf::Config * global_conf;
extern std::string share_directory;
extern std::string etc_directory;
extern std::string var_directory;
extern std::string socket_name;
extern std::vector<std::string> rulesets;
extern bool exit_flag;
extern bool daemon_flag;
extern bool restricted_flag;
extern int timeoffset;
extern int client_port_num;
extern int slave_port_num;
extern int peer_port_num;

int loadConfig(int argc, char ** argv, bool server = false);

#endif // COMMON_GLOBALS_H
