// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include <varconf/Config.h>

#include <rulesets/Python_API.h>

#include <common/globals.h>

int main(int argc, char ** argv)
{
    global_conf = varconf::Config::inst();

    if (loadConfig(argc, argv)) {
        // Fatal error loading config file
        return 1;
    }

    std::string ruleset;
    while (global_conf->findItem("cyphesis", "ruleset")) {
        ruleset = global_conf->getItem("cyphesis", "ruleset");
        global_conf->erase("cyphesis", "ruleset");
        cout << "Reading in " << ruleset << endl << flush;
        global_conf->readFromFile(share_directory + "/cyphesis/" + ruleset + ".vconf");
        rulesets.push_back(ruleset);
    };

    init_python_api();

    try {
        // if init.display:;
            // print "Use --nodisplay to hide 'text graphics'";
        ObserverClient & observer = *new ObserverClient();
        if (!observer.setup()) {
            return 1;
        }
        observer.loadDefault();
        //observer.run();
    }
    catch (...) {
        std::cerr << "EMERGENCY: cyphesis_client: Exception caught in main; exiting" << std::endl << std::flush;
    }
}
