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


/// \page cyconfig_index
///
/// \section Introduction
///
/// cyconfig is a non-interactive commandline tool to control and modify
/// cyphesis configuration. For information on the usage, please see the unix
/// manual page. The manual page is generated from docbook sources, so can
/// also be converted into other formats.


#include "common/globals.h"

#include <varconf/config.h>

#include <cstring>

#include <cassert>
#include <basedir.h>
#include <boost/filesystem/path.hpp>

static int install(int argc, char ** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <ruleset> [ <directory> | <zipfile> ]" << std::endl << std::flush;
        return 1;
    }
    return 0;
}

/// \brief Entry in the global command table for cyconfig
struct config_command {
    const char * cmd_string;
    int (*cmd_function)(int argc, char ** argv);
    const char * cmd_descrption;
};

static struct config_command commands[] = {
    { "install", &install, "Install a ruleset", },
    { 0, }
};

static int runCommand(int argc, char ** argv)
{
    assert(argc > 0);
    for (struct config_command * i = &commands[0]; i->cmd_string != 0; ++i) {
        if (strcmp(argv[0], i->cmd_string) == 0) {
            return i->cmd_function(argc, argv);
        }
    }
    return 1;
}

int main(int argc, char ** argv)
{
    const auto* configHome = xdgConfigHome(nullptr);

    auto homeDirConfig= boost::filesystem::path(configHome) / "cyphesis.vconf";
    auto config_instance = varconf::Config::inst();
    config_instance->readFromFile(homeDirConfig.string());
    int optindex = config_instance->getCmdline(argc, argv);

    if (config_instance->findItem("", "version")) {
        reportVersion(argv[0]);
        return 0;
    }

    if (config_instance->findItem("", "help")) {
        showUsage(argv[0], USAGE_SERVER);
        return 0;
    }

    if (optindex > 0 && optindex < argc) {
        runCommand(argc - optindex, &argv[optindex]);
    }

    config_instance->writeToFile(homeDirConfig.string(), (varconf::Scope)(varconf::USER | varconf::INSTANCE));
    return 0;
}
