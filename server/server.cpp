// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <netdb.h>
}

#include <common/config.h>
#include <common/const.h>
#include <common/log.h>
#include <common/debug.h>
#include <common/persistance.h>
#include <common/utility.h>
#include <common/globals.h>

#include <fstream>

#include "ServerRouting_methods.h"
#include "Connection.h"
#include "CommClient.h"
#include "CommServer.h"

static const bool debug_flag = false;

void init_python_api();

static inline char *pack_uint32(uint32_t data, char *buffer, unsigned int *size)
{
    uint32_t netorder;

    netorder = htonl(data);
    memcpy(buffer, &netorder, sizeof(uint32_t));
    *size += sizeof(uint32_t);
    return buffer+sizeof(uint32_t);
}

static inline char *unpack_uint32(uint32_t *dest, char *buffer)
{
    uint32_t netorder;

    memcpy(&netorder, buffer, sizeof(uint32_t));
    *dest = ntohl(netorder);
    return buffer+sizeof(uint32_t);
}

CommClient::CommClient(CommServer & svr, int fd, int port) :
            commServer(svr),
            clientFd(fd), clientBuf(fd), clientIos(&clientBuf),
            connection(*new Connection(*this))
{
}


CommClient::~CommClient()
{
    connection.destroy();
    delete &connection;
    if (encoder != NULL) {
        delete encoder;
    }
}

int CommClient::setup()
{
    Atlas::Net::StreamAccept accept("cyphesis " + commServer.identity, clientIos, this);

    debug(cout << "Negotiating... " << flush;);
    while (accept.GetState() == Atlas::Net::StreamAccept::IN_PROGRESS) {
        accept.Poll();
    }
    debug(cout << "done" << endl;);

    if (accept.GetState() == Atlas::Net::StreamAccept::FAILED) {
        cerr << "Failed to negotiate" << endl;
        return 0;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = accept.GetCodec();

    // This should always be sent at the beginning of a session

    encoder = new Atlas::Objects::Encoder(codec);

    codec->StreamBegin();

    return 1;
}

void CommClient::message(const RootOperation & op)
{
    oplist reply = connection.message(op);
    for(oplist::const_iterator I = reply.begin(); I != reply.end(); I++) {
        debug(cout << "sending reply" << endl << flush;);
        send(*I);
        delete *I;
    }
}

void CommClient::UnknownObjectArrived(const Object& o)
{
    debug(cout << "An unknown has arrived." << endl << flush;);
    RootOperation * r = (RootOperation*)utility::Object_asRoot(o);
    if (r != NULL) {
        message(*r);
    }
#if 0
    debug(cout << "An unknown has arrived." << endl << flush;);
    if (o.IsMap()) {
        for(Object::MapType::const_iterator I = o.AsMap().begin();
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

void CommClient::ObjectArrived(const Login & op)
{
    debug(cout << "A login operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Create & op)
{
    debug(cout << "A create operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Move & op)
{
    debug(cout << "A move operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Set & op)
{
    debug(cout << "A set operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Touch & op)
{
    debug(cout << "A touch operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Look & op)
{
    debug(cout << "A look operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Talk & op)
{
    debug(cout << "A talk operation thingy here!" << endl << flush;);
    message(op);
}

void CommClient::ObjectArrived(const Get & op)
{
    debug(cout << "A get operation thingy here!" << endl << flush;);
    message(op);
}

bool CommServer::useMetaserver = true;

CommServer::CommServer(const string & ident) :
              metaserverTime(-1),
              identity(ident), server(*new ServerRouting(*this, ident))
{
}

int CommServer::setup(int port)
{
    struct sockaddr_in sin;

    serverPort = port;
    serverFd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        return -1;
    }
    int flag=1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0L;
    if (bind(serverFd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(serverFd);
        return -1;
    }
    listen(serverFd, 5);
    // server=new ServerRouting(this,identity);

    if (!useMetaserver) {
        return 0;
    }
    // Establish stuff for metaserver

    memset(&meta_sa, 0, sizeof(meta_sa));
    meta_sa.sin_family = AF_INET;
    meta_sa.sin_port = htons(metaserverPort);

    std::string mserver("metaserver.worldforge.org");

    if (global_conf->findItem("cyphesis", "metaserver")) {
        mserver = global_conf->getItem("cyphesis", "metaserver");
    }
    cout << "Connecting to metaserver..." << endl << flush;
    struct hostent * ms_addr = gethostbyname(mserver.c_str());
    if (ms_addr == NULL) {
        cerr << "metaserver lookup failed. Disabling metaserver." <<endl<<flush;
        useMetaserver = false;
        return 0;
    }
    memcpy(&meta_sa.sin_addr, ms_addr->h_addr_list[0], ms_addr->h_length);
    
    metaFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (metaFd < 0) {
        cerr << "WARNING: Could not create metaserver connection" <<endl<<flush;
        useMetaserver = false;
        perror("socket");
    }

    return 0;
}

int CommServer::accept()
{
    struct sockaddr_in sin;
    unsigned int addr_len = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(serverPort);
    sin.sin_addr.s_addr = 0L;

    debug(cout << "Accepting.." << endl << flush;);
    int asockfd = ::accept(serverFd, (struct sockaddr *)&sin, &addr_len);

    if (asockfd < 0) {
        return -1;
    }
    debug(cout << "Accepted" << endl << flush;);
    CommClient * newcli = new CommClient(*this, asockfd, sin.sin_port);
    if (newcli->setup()) {
        clients.insert(std::pair<int, CommClient *>(asockfd, newcli));
    }
    return 0;
}

inline void CommServer::idle()
{
    time_t ctime = time(NULL);
    if ((ctime > (metaserverTime + 5 * 60)) && useMetaserver) {
        cout << "Sending keepalive" << endl << flush;
        metaserverTime = ctime;
        metaserverKeepalive();
    }
    server.idle();
}

void CommServer::loop()
{
    fd_set sock_fds;
    int highest;
    int client_fd;
    CommClient * client;
    struct timeval tv;

    tv.tv_sec=0;
    tv.tv_usec=100000;

    FD_ZERO(&sock_fds);

    FD_SET(serverFd, &sock_fds);
    if (useMetaserver) {
        FD_SET(metaFd, &sock_fds);
        highest = max(serverFd, metaFd);
    } else {
        highest = serverFd;
    }
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
               removeClient(client);
               break;
           } else {
               cerr << "FATAL THIS SHOULD NEVER HAPPEN" << endl << flush;
               removeClient(client);
               break;
           }
       }
    }
    if (FD_ISSET(serverFd, &sock_fds)) {
        debug(cout << "selected on server" << endl << flush;);
        accept();
    }
    if (useMetaserver && FD_ISSET(metaFd, &sock_fds)) {
        debug(cout << "selected on metaserver" << endl << flush;);
        metaserverReply();
    }
    idle();
}

inline void CommServer::removeClient(CommClient * client, char * error_msg)
{
    Object::MapType err;
    err["message"] = Object(error_msg);
    Object::ListType eargs(1,Object(err));

    Error * e = new Error(Error::Instantiate());

    e->SetArgs(eargs);

    // Need to deal with cleanly sending the error op, without hanging
    // if the client has already gone. FIXME
    
    if (client) {
        client->send(e);
        clients.erase(client->getFd());
    }
    delete e;
    delete client;
}

void CommServer::removeClient(CommClient * client)
{
    removeClient(client,"You caused exception. Connection closed");
}

#define MAXLINE 4096

void CommServer::metaserverKeepalive()
{
    char         mesg[MAXLINE];
    unsigned int packet_size=0;

    pack_uint32(SKEEP_ALIVE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size,0, (sockaddr *)&meta_sa, sizeof(meta_sa));
}

void CommServer::metaserverReply()
{
    char                mesg[MAXLINE];
    char               *mesg_ptr;
    uint32_t            handshake=0, command=0;
    struct sockaddr	addr;
    socklen_t           addrlen;
    unsigned int        packet_size;

    if (recvfrom(metaFd, mesg, MAXLINE, 0, &addr, &addrlen) < 0) {
        cerr << "WARNING: No reply from metaserver" << endl << flush;
        return;
    }
    mesg_ptr = unpack_uint32(&command, mesg);

    if(command == HANDSHAKE)
    {
        mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
        cout << "Server contacted successfully." << endl << flush;

        packet_size = 0;
        mesg_ptr = pack_uint32(SERVERSHAKE, mesg, &packet_size);
        mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);

        sendto(metaFd,mesg,packet_size,0,(sockaddr*)&meta_sa,sizeof(meta_sa));
    }

}

void CommServer::metaserverTerminate()
{
    char         mesg[MAXLINE];
    unsigned int packet_size=0;

    pack_uint32(TERMINATE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size, 0, (sockaddr *)&meta_sa, sizeof(meta_sa));
}

#include <rulesets/EntityFactory.h>

int main(int argc, char ** argv)
{
    global_conf = varconf::Config::inst();

    if (install_directory=="NONE") {
        install_directory = "/usr/local";
    }

    Persistance::init();

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
        EntityFactory::instance()->readRuleset(install_directory + "/share/cyphesis/" + ruleset);
        rulesets.push_back(ruleset);
    };

    if (global_conf->findItem("cyphesis", "restricted")) {
        Persistance::restricted=global_conf->getItem("cyphesis","restricted");
        if (Persistance::restricted) {
            cout << "Running in restricted mode" << endl << flush;
        }
    }
    bool use_metaserver = true;
    if (global_conf->findItem("cyphesis", "usemetaserver")) {
        use_metaserver=global_conf->getItem("cyphesis","usemetaserver");
    }

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
    CommServer s(rulesets.front());
    s.useMetaserver = use_metaserver;
    if (s.setup(6767)) {
        cerr << "Could not create listen socket." << endl << flush;
        exit(1);
    }
    if (consts::debug_level>=1) {
        char * log_name="cyphesis_world.log";
        cout << "consts::debug_level>=1:, logging to" << log_name << endl;
        //s.server.world.queue_fp.open(log_name,ios::out);
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
    cout << "Performing clean shutdown..." << endl << flush;
    Persistance::shutdown();
    s.metaserverTerminate();
    cout << "Clean shutdown complete." << endl << flush;
    return 0;
}
