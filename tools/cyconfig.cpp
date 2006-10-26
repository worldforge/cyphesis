// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: cyconfig.cpp,v 1.7 2006-10-26 00:48:16 alriddoch Exp $

#include <varconf/config.h>

#include <cassert>

static int install(int argc, char ** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <ruleset> [ <directory> | <zipfile> ]" << std::endl << std::flush;
        return 1;
    }
    return 0;
}

struct command {
    const char * cmd_string;
    int (*cmd_function)(int argc, char ** argv);
    const char * cmd_descrption;
};

static struct command commands[] = {
    { "install", &install, "Install a ruleset", },
    { 0, }
};

static int runCommand(int argc, char ** argv)
{
    assert(argc > 0);
    for (struct command * i = &commands[0]; i->cmd_string != 0; ++i) {
        if (strcmp(argv[0], i->cmd_string) == 0) {
            return i->cmd_function(argc, argv);
        }
    }
    return 1;
}

int main(int argc, char ** argv)
{
    char * home;
    if ((home = getenv("HOME")) == NULL) {
        std::cerr << "ERROR: Unable to get home directory." << std::endl << std::flush;
        return 1;
    }
    std::string homeDirConfig = std::string(home) + "/.cyphesis.vconf";
    varconf::Config * global_conf = varconf::Config::inst();
    global_conf->readFromFile(homeDirConfig);
    int optind = global_conf->getCmdline(argc, argv);

    if (optind > 0 && optind < argc) {
        runCommand(argc - optind, &argv[optind]);
    }

    global_conf->writeToFile(homeDirConfig, (varconf::Scope)(varconf::USER | varconf::INSTANCE));
    return 0;
}
