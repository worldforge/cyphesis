// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Codec.h>
#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Encoder.h>

#include <varconf/Config.h>

#include <iostream.h>
#include <fstream.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <sys/stat.h>
    #include <unistd.h>
}

#include "ClientConnection.h"

#include <common/debug.h>
#include <common/globals.h>

static bool debug_flag = false;

char * getHome()
{
    char * dir = getenv("HOME");
    if ((dir != NULL) && (strlen(dir) != 0)) {
        return dir;
    }
    return NULL;
}

#include <rulesets/MindFactory.h>

int main(int argc, char ** argv)
{
    std::string server = "localhost";

    if (install_directory=="NONE") {
        install_directory = "/usr/local";
    }

    // See if the user has set the install directory on the command line
    char * home;
    if ((home = getenv("HOME")) != NULL) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);
    if (global_conf->findItem("cyphesis", "directory")) {
        install_directory = global_conf->getItem("cyphesis", "directory");
    }
    global_conf->readFromFile(install_directory + "/share/cyphesis/cyphesis.vconf");
    if (home != NULL) {
        global_conf->readFromFile(std::string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);

    std::string ruleset;
    while (global_conf->findItem("cyphesis", "ruleset")) {
        ruleset = global_conf->getItem("cyphesis", "ruleset");
        global_conf->erase("cyphesis", "ruleset");
        cout << "Reading in " << ruleset << endl << flush;
        MindFactory::instance()->readRuleset(install_directory + "/share/cyphesis/" + ruleset);
        rulesets.push_back(ruleset);
    };


    if (global_conf->findItem("aiclient", "server")) {
        server = global_conf->getItem("aiclient", "server");
    }
    ClientConnection s;
    if (!s.connect(server)) {
        cerr << "Could not connect to server." << endl << flush;
        return 1;
    }
    s.login("ai", "pwd");
    if (s.wait()) {
        cerr << "Login failed." << endl << flush;
        return 1;
    }
    while (!exit_flag) {
        try {
            s.loop();
        }
        catch (...) {
            cerr << "*********** EMERGENCY ***********" << endl;
            cerr << "EXCEPTION: Caught in main()" << endl;
            cerr << "         : Continuing..." << endl;
        }
    }
}
