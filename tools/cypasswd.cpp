// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Message/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>

#include <config.h>
#include <db3/db_cxx.h>

#include <string>

#ifdef HAVE_SSTREAM_H
#include <sstream.h>
#else
#include "sstream.h"
#endif


#include <signal.h>

// This is the currently very basic password management tool, which can
// be used to control the passwords of accounts in the main servers account
// database. This is the only way to create accounts on a server in
// restricted mode.

// TODO: Make sure the rest of the Object is preserver, rather than just
//       blatting it with a new Object.

class Decoder : public Atlas::Message::DecoderBase {
    virtual void ObjectArrived(const Atlas::Message::Object& obj) { }
};

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

void acput(Db & db, const string & name, const string & password)
{
    Decoder dec;
    std::stringstream str;
    Atlas::Codecs::XML codec(str, &dec);
    Atlas::Message::Encoder enc(&codec);

    Atlas::Message::Object::MapType ac_map;
    ac_map["id"] = name;
    ac_map["password"] = password;

    codec.StreamBegin();
    enc.StreamMessage(ac_map);
    codec.StreamEnd();

    Dbt key, data;
    key = Dbt((void*)name.c_str(), name.size() + 1);
    data = Dbt((void*)str.str().c_str(), str.str().size() + 1);
    db.put(NULL, &key, &data, 0);
}

int main(int argc, char ** argv)
{
#ifndef HAVE_LIBDB_CXX
    std::cerr << "This version of cyphesis was built without persistant account support" << endl << flush;
    exit(0);
#endif
    string acname;
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
    
    Db db(NULL, DB_CXX_NO_EXCEPTIONS);

    if (db.open("/var/forge/cyphesis/db","account",DB_BTREE,DB_CREATE,0600)) {
        std::cerr << "Failed to open password database" << endl << flush;
    }
    
    Dbt key((void*)"admin", 6);
    Dbt data;

    int res = db.get(NULL, &key, &data, 0);

    if (res == DB_NOTFOUND) {
        std::cout << "Admin account does not yet exist" << endl << flush;
        acname = "admin";
        action = ADD;
    }
    if (action != ADD) {
        key = Dbt((void*)acname.c_str(), acname.size() +1);
        data = Dbt();
        res = db.get(NULL, &key, &data, 0);
        if (res == DB_NOTFOUND) {
            std::cout<<"Account "<<acname<<" does not yet exist"<<endl<<flush;
            db.close(0);
            return 0;
        }
    }
    if (action == DEL) {
        key = Dbt((void*)acname.c_str(), acname.size() +1);
        db.del(NULL, &key, 0);
        cout << "Account " << acname << " removed." << endl << flush;
        db.close(0);
        return 0;
    }
    std::string password, password2;
    std::cout << "New " << acname << " password:" << flush;
    std::cin >> password;
    std::cout << "Retype " << acname << " password:" << flush;
    std::cin >> password2;
    if (password == password2) {
        acput(db, acname, password);
        std::cout << "Password changed." << endl << std::flush;
    } else {
        std::cout << "Passwords did not match. Account database unchanged."
                  << endl << std::flush;
    }
    db.close(0);
}
