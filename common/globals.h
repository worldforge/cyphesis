// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef COMMON_GLOBALS_H
#define COMMON_GLOBALS_H

#include <string>
#include <list>

#include <varconf/Config.h>

extern varconf::Config * global_conf;
extern std::string install_directory;
extern std::list<std::string> rulesets;
extern bool exit_flag;

#endif // COMMON_GLOBALS_H
