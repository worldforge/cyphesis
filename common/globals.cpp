// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "globals.h"
#include "config.h"

varconf::Config * global_conf = NULL;
std::string install_directory(INSTALLDIR);
std::list<std::string> rulesets;
bool exit_flag;
