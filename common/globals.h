// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

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
extern std::vector<std::string> rulesets;
extern bool exit_flag;
extern bool daemon_flag;
extern int timeoffset;
extern int port_num;

bool loadConfig(int argc, char ** argv, bool server = false);

#endif // COMMON_GLOBALS_H
