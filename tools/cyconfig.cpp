// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <varconf/Config.h>

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

    global_conf->writeToFile(homeDirConfig);
    return 0;
}
