// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <common/accountbase.h>
#include <common/globals.h>

#include <string>

#include <signal.h>

// This is the currently very basic password management tool, which can
// be used to control the passwords of accounts in the main servers account
// database. This is the only way to create accounts on a server in
// restricted mode.

// TODO: Make sure the rest of the Object is preserver, rather than just
//       blatting it with a new Object.

using Atlas::Message::Object;

#define ADD 0
#define SET 1
#define DEL 2

void usage(char * n)
{
    // Don't call this once the database is open. It doesn't return so the
    // database will not be closed.
    std::cout << "usage: " << n << " -[asd] account" << endl << flush;
    exit(0);
}

int main(int argc, char ** argv)
{
    if (loadConfig(argc, argv)) {
        // Fatal error loading config file
        return 1;
    }

    std::string acname;
    int action;

    if (argc == 1) {
        acname = "admin";
        action = SET;
    } else if (argc == 3) {
        if (argv[1][0] == '-') {
            int c = argv[1][1];
            if (c == 'a') {
                action = ADD;
            } else if (c == 's') {
                action = SET;
            } else if (c == 'd') {
                action = DEL;
            } else {
                usage(argv[0]);
            }
            acname = argv[2];
        } else {
            usage(argv[0]);
        }
    } else {
        usage(argv[0]);
    }

    // Once the database is open, we must not get signals as that will
    // cause the program to exit without cleaning up the database.
    signal(SIGINT, SIG_IGN);
    
    //Db db(NULL, DB_CXX_NO_EXCEPTIONS);

    //if (db.open("/var/forge/cyphesis/db","account",DB_BTREE,DB_CREATE,0600)) {
        //std::cerr << "Failed to open password database" << endl << flush;
    //}
    
    //Dbt key((void*)"admin", 6);
    //Dbt data;

    AccountBase * db = AccountBase::instance(true);

    Object::MapType data;

    bool res = db->getAccount("admin", data);

    if (!res) {
        std::cout << "Admin account does not yet exist" << endl << flush;
        acname = "admin";
        action = ADD;
    }
    if (action != ADD) {
        Object::MapType o;
        res = db->getAccount(acname, o);
        if (!res) {
            std::cout<<"Account "<<acname<<" does not yet exist"<<endl<<flush;
            AccountBase::del();
            return 0;
        }
    }
    if (action == DEL) {
        db->delAccount(acname);
        if (res) {
            cout << "Account " << acname << " removed." << endl << flush;
        }
        AccountBase::del();
        return 0;
    }
    std::string password, password2;
    std::cout << "New " << acname << " password:" << flush;
    std::cin >> password;
    std::cout << "Retype " << acname << " password:" << flush;
    std::cin >> password2;
    if (password == password2) {
        Object::MapType amap;
        amap["id"] = acname;
        amap["password"] = password;

        res = db->putAccount(amap, acname);
        if (res) {
            std::cout << "Password changed." << endl << std::flush;
        }
    } else {
        std::cout << "Passwords did not match. Account database unchanged."
                  << endl << std::flush;
    }
    AccountBase::del();
}
