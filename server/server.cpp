// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Decoder.h>

#include <varconf/Config.h>

#include <iostream.h>
#include <fstream.h>

#include <Python.h>


extern "C" {
    #include <stdio.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
}

#include <common/config.h>
#include <common/const.h>
#include <common/log.h>
#include <common/debug.h>

#include <fstream>

#include "ServerRouting.h"
#include "Connection.h"
#include "server.h"
#include "CommClient.h"
#include "CommServer.h"

int profile_flag=0;
static const bool debug_flag = false;

string install_directory = string(INSTALLDIR);

using namespace Atlas;

void init_python_api();

CommClient::~CommClient() {
    if (connection != NULL) {
        connection->destroy();
        delete connection;
    }
    if (encoder != NULL) {
        delete encoder;
    }
}

int CommClient::setup()
{
    Atlas::Net::StreamAccept accept("cyphesis", client_ios, this);

    debug(cout << "Negotiating... " << flush;);
    while (accept.GetState() == Negotiate<iostream>::IN_PROGRESS) {
        accept.Poll();
    }
    debug(cout << "done" << endl;);

    if (accept.GetState() == Negotiate<iostream>::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return(0);
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = accept.GetCodec();

    // This should always be sent at the beginning of a session

    encoder = new Objects::Encoder(codec);

    codec->StreamBegin();

    connection=new Connection(this);
    return(1);
}

void CommClient::message(const Objects::Operation::RootOperation & op)
{
    oplist reply = connection->message(op);
    while (reply.size() != 0) {
        Objects::Operation::RootOperation * rep_op = reply.front();
        debug(cout << "sending reply" << endl << flush;);
        send(rep_op);
        delete rep_op;
        reply.pop_front();
    }
}

void CommClient::UnknownObjectArrived(const Atlas::Message::Object& o)
{
#if 0
    debug(cout << "An unknown has arrived." << endl << flush;);
    if (o.IsMap()) {
        for(Message::Object::MapType::const_iterator I = o.AsMap().begin();
		I != o.AsMap().end();
		I++) {
		debug(cout << I->first << endl << flush;);
                if (I->second.IsString()) {
		    debug(cout << I->second.AsString() << endl << flush;);
                }
	}
    } else {
        debug(cout << "Its not a map." << endl << flush;);
    }
#endif
}

void CommClient::ObjectArrived(const Objects::Operation::Login & op)
{
    debug(cout << "A login operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Create & op)
{
    debug(cout << "A create operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Move & op)
{
    debug(cout << "A move operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Set & op)
{
    debug(cout << "A set operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Touch & op)
{
    debug(cout << "A touch operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Look & op)
{
    debug(cout << "A look operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Objects::Operation::Talk & op)
{
    debug(cout << "A talk operation thingy here!" << endl << flush;);
    message(op);
}

int CommServer::setup(int port)
{
    struct sockaddr_in sin;

    server_port = port;
    server_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        return(-1);
    }
    int flag=1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0L;
    if (bind(server_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(server_fd);
        return(-1);
    }
    listen(server_fd, 5);
    server=new ServerRouting(this,"server");
    return(0);
}

int CommServer::accept() {
    struct sockaddr_in sin;
    unsigned int addr_len = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = 0L;

    debug(cout << "Accepting.." << endl << flush;);
    int asockfd = ::accept(server_fd, (struct sockaddr *)&sin, &addr_len);

    if (asockfd < 0) {
        return(-1);
    }
    debug(cout << "Accepted" << endl << flush;);
    CommClient * newcli = new CommClient(this, asockfd, sin.sin_port);
    if (newcli->setup()) {
        clients.insert(std::pair<int, CommClient *>(asockfd, newcli));
    }
    return(0);
}

inline void CommServer::idle() {
    server->idle();
}

void CommServer::loop() {
    fd_set sock_fds;
    int highest = server_fd;
    int client_fd;
    CommClient * client;
    struct timeval tv;

    tv.tv_sec=0;
    tv.tv_usec=100000;

    FD_ZERO(&sock_fds);

    FD_SET(server_fd, &sock_fds);
    client_map_t::const_iterator I;
    for(I = clients.begin(); I != clients.end(); I++) {
       client_fd = I->first;
       FD_SET(client_fd, &sock_fds);
       if (client_fd > highest) {
           highest=client_fd;
       }
    }
    highest++;
    int rval = select(highest, &sock_fds, NULL, NULL, &tv);

    if (rval < 0) {
        return;
    }
    
    for(I = clients.begin(); I != clients.end(); I++) {
       client_fd = I->first;
       if (FD_ISSET(client_fd, &sock_fds)) {
           client = I->second;
           if (client->peek() != -1) {
               client->read();
           } else if (client->eof()) {
               remove_client(client);
               break;
           } else {
               cerr << "FATAL THIS SHOULD NEVER HAPPEN" << endl << flush;
               remove_client(client);
               break;
           }
       }
    }
    if (FD_ISSET(server_fd, &sock_fds)) {
        debug(cout << "selected on server" << endl << flush;);
        accept();
    }
    idle();
}

inline void CommServer::remove_client(CommClient * client, char * error_msg)
{
    Message::Object::MapType err;
    err["message"] = Message::Object(error_msg);
    Message::Object::ListType eargs(1,Message::Object(err));

    Error * e = new Error();
    *e = Error::Instantiate();

    e->SetArgs(eargs);

    // Need to deal with cleanly sending the error op, without hanging
    // if the client has already gone. FIXME
    
    if (client) {
        client->send(e);
        clients.erase(client->get_fd());
    }
    delete e;
    delete client;
}

void CommServer::remove_client(CommClient * client)
{
    remove_client(client,"You caused exception. Connection closed");
}

varconf::Config * global_conf = varconf::Config::inst();

list<string> rulesets;

#include <rulesets/ThingFactory.h>

int main(int argc, char ** argv)
{
    if (install_directory=="NONE") {
        install_directory = "/usr/local";
    }

    // See if the user has set the install directory on the command line
    char * home;
    if ((home = getenv("HOME")) != NULL) {
        global_conf->readFromFile(string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);
    if (global_conf->findItem("cyphesis", "directory")) {
        install_directory = global_conf->getItem("cyphesis", "directory");
        if (home != NULL) {
            global_conf->writeToFile(string(home) + "/.cyphesis.vconf");
        }
    }
    global_conf->readFromFile(install_directory + "/share/cyphesis/cyphesis.vconf");
    if ((home = getenv("HOME")) != NULL) {
        global_conf->readFromFile(string(home) + "/.cyphesis.vconf");
    }
    global_conf->getCmdline(argc, argv);
    string ruleset;
    while (global_conf->findItem("cyphesis", "ruleset")) {
        ruleset = global_conf->getItem("cyphesis", "ruleset");
        global_conf->erase("cyphesis", "ruleset");
        cout << "Reading in " << ruleset << endl << flush;
        thing_factory.readRuleset(install_directory + "/share/cyphesis/" + ruleset);
        rulesets.push_back(ruleset);
    };
    

    init_python_api();
    cout << Py_GetPath() << endl << flush;

    if (consts::debug_level>=1) {
        cout << "consts::debug_level>=1, logging to cyphesis_server*.log files" << endl << flush;
	//ofstream log_stream("cyphesis_server.log",ios::out);
        common::log::inform_fp.open("cyphesis_server.log",ios::out);
        common::log::debug_fp.open("cyphesis_server_debug.log",ios::out);
    }
    if (consts::debug_thinking>=1) {
        char * log_name="thinking.log";
        cout << "consts::debug_thinking>=1:, logging to" << log_name << endl;
        common::log::thinking_fp.open(log_name,ios::out);
    }
    CommServer s;
    if (s.setup(6767)) {
        cerr << "Could not create listen socket." << endl << flush;
        exit(1);
    }
    if (consts::debug_level>=1) {
        char * log_name="cyphesis_world.log";
        cout << "consts::debug_level>=1:, logging to" << log_name << endl;
        //s.server.world.queue_fp.open(log_name,ios::out);
    }
    int exit_flag=0;
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
