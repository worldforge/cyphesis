// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <varconf/Config.h>

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
    global_conf->getCmdline(argc, argv);
    global_conf->writeToFile(homeDirConfig);
    return 0;
}
