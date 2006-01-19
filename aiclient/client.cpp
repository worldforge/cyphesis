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


#include "ClientConnection.h"

#include "rulesets/MindFactory.h"

#include "common/debug.h"
#include "common/globals.h"

#include <Atlas/Codec.h>
#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Message/Encoder.h>

#include <varconf/config.h>

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

static bool debug_flag = false;

char * getHome()
{
    char * dir = getenv("HOME");
    if ((dir != NULL) && (strlen(dir) != 0)) {
        return dir;
    }
    return NULL;
}

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
