// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "globals.h"

varconf::Config * global_conf = NULL;
std::string share_directory = SHAREDIR;
std::list<std::string> rulesets;
bool exit_flag = false;
bool daemon_flag = false;
int timeoffset = 0;
