// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ObserverClient.h"

#include <rulesets/Python_API.h>

#include <common/globals.h>

int main(int argc, char ** argv)
{
    if (loadConfig(argc, argv)) {
        // Fatal error loading config file
        return 1;
    }

    init_python_api();

    try {
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
