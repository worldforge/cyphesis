// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommServer.h"

#include "CommClient.h"
#include "CommListener.h"
#include "CommMetaClient.h"
#include "ServerRouting_methods.h"
#include "protocol_instructions.h"

#include <common/log.h>
#include <common/debug.h>

#include <varconf/Config.h>

#include <iostream>

static const bool debug_flag = false;

#if 0
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
#endif

CommServer::CommServer(const std::string & ruleset, const std::string & ident) :
              metaserverTime(-1), metaClient(* new CommMetaClient(*this)),
              useMetaserver(true), identity(ident),
              server(*new ServerRouting(*this, ruleset, ident))
{
}

CommServer::~CommServer()
{
    comm_set_t::const_iterator I = sockets.begin();
    for(; I != sockets.end(); I++) {
        delete *I;
    }
    delete &server;
}

bool CommServer::setup(int port)
{
    CommListener * cl = new CommListener(*this);

    if (!cl->setup(port)) {
	return false;
    }

    add(cl);

    if (global_conf->findItem("cyphesis", "usemetaserver")) {
        useMetaserver = global_conf->getItem("cyphesis","usemetaserver");
    }

    if (!useMetaserver) {
        return true;
    }

    std::string mserver("metaserver.worldforge.org");

    if (global_conf->findItem("cyphesis", "metaserver")) {
        mserver = global_conf->getItem("cyphesis", "metaserver");
    }

    if (metaClient.setup(mserver)) {
	add(&metaClient);
    } else {
	useMetaserver = false;
    }
    return true;
}

void CommServer::shutdown()
{
    if (useMetaserver) {
	metaClient.metaserverTerminate();
    }
}

#if 0
{
    // Nasty low level socket code to set up listen socket. This should be
    // replaced with a socket class library.
    struct sockaddr_in sin;

    serverPort = port;
    serverFd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        return false;
    }
    int flag = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0L;
    if (bind(serverFd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(serverFd);
        return false;
    }
    listen(serverFd, 5);

    // If we are not going to use the metaserver, then out work here is
    // complete.
    if (!useMetaserver) {
        return true;
    }

    // Establish socket for communication with the metaserver
    memset(&meta_sa, 0, sizeof(meta_sa));
    meta_sa.sin_family = AF_INET;
    meta_sa.sin_port = htons(metaserverPort);

    std::string mserver("metaserver.worldforge.org");

    if (global_conf->findItem("cyphesis", "metaserver")) {
        mserver = global_conf->getItem("cyphesis", "metaserver");
    }
    debug(std::cout << "Connecting to metaserver..." << std::endl << std::flush;);
    struct hostent * ms_addr = gethostbyname(mserver.c_str());
    if (ms_addr == NULL) {
        log(WARNING, "metaserver lookup failed. Disabling metaserver");
        useMetaserver = false;
        return true;
    }
    memcpy(&meta_sa.sin_addr, ms_addr->h_addr_list[0], ms_addr->h_length);
    
    metaFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (metaFd < 0) {
        log(WARNING, "Could not connect to metaserver. Disabling metaserver");
        useMetaserver = false;
        perror("socket");
    }

    return true;
}

bool CommServer::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_in sin;
    unsigned int addr_len = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(serverPort);
    sin.sin_addr.s_addr = 0L;

    debug(std::cout << "Accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(serverFd, (struct sockaddr *)&sin, &addr_len);

    if (asockfd < 0) {
        return false;
    }
    debug(std::cout << "Accepted" << std::endl << std::flush;);
    CommClient * newcli = new CommClient(*this, asockfd, sin.sin_port);

    newcli->setup();

    // Add this new client to the list.
    sockets.insert(newcli);

    return true;
}
#endif

inline void CommServer::idle()
{
    // Update the time, and get the core server object to process
    // stuff.
    time_t ctime = time(NULL);
    if ((ctime > (metaserverTime + 5 * 60)) && useMetaserver) {
        debug(std::cout << "Sending keepalive" << std::endl << std::flush;);
        metaserverTime = ctime;
        metaClient.metaserverKeepalive();
    }
    // server.idle() is inlined, and simply calls the world idle method,
    // which is not directly accessible from here.
    server.idle();
}

void CommServer::loop()
{
    // This is the main code loop.
    // Classic select code for checking incoming data or client connections.
    // It may be beneficial to re-write this code to use the poll(2) system
    // call.
    fd_set sock_fds;
    int highest;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    FD_ZERO(&sock_fds);

#if 0
    FD_SET(serverFd, &sock_fds);
    if (useMetaserver) {
        FD_SET(metaFd, &sock_fds);
        highest = std::max(serverFd, metaFd);
    } else {
        highest = serverFd;
    }
#endif

    comm_set_t::const_iterator I;
    for(I = sockets.begin(); I != sockets.end(); I++) {
       if (!(*I)->isOpen()) { continue; }
       int client_fd = (*I)->getFd();
       FD_SET(client_fd, &sock_fds);
       if (client_fd > highest) {
           highest = client_fd;
       }
    }
    highest++;
    int rval = select(highest, &sock_fds, NULL, NULL, &tv);

    if (rval < 0) {
        return;
    }
    
    std::set<CommSocket *> obsoleteConnections;
    for(I = sockets.begin(); I != sockets.end(); I++) {
       CommSocket * client = *I;
       if (!client->isOpen()) {
           obsoleteConnections.insert(client);
           continue;
       }
       if (FD_ISSET(client->getFd(), &sock_fds)) {
           if (!client->eof()) {
               if (client->read()) {
                   debug(std::cout << "Removing client due to failed negotiation or timeout" << std::endl << std::flush;);
                   obsoleteConnections.insert(client);
               }
           } else {
               // It is not clear why but on some implementation/circumstances
               // client->eof() is true, and sometimes it isn't.
               // Either way, the stream is now done, and we should remove it
               obsoleteConnections.insert(client);
           }
       }
    }
#if 0
    if (FD_ISSET(serverFd, &sock_fds)) {
        debug(std::cout << "selected on server" << std::endl << std::flush;);
        accept();
    }
    if (useMetaserver && FD_ISSET(metaFd, &sock_fds)) {
        debug(std::cout << "selected on metaserver" << std::endl << std::flush;);
        metaserverReply();
    }
#endif
    std::set<CommSocket *>::const_iterator J = obsoleteConnections.begin();
    for(; J != obsoleteConnections.end(); ++J) {
        removeSocket(*J);
    }
    // Once we have done all socket related stuff, proceed with processing
    // the world.
    idle();
}

inline void CommServer::removeSocket(CommSocket * client, char * error_msg)
{
    // FIXME This code needs to be moved into CommClient
    // Atlas::Message::Object::MapType err;
    // err["message"] = error_msg;
    // Atlas::Message::Object::ListType eargs(1,err);

    // Error e(Error::Instantiate());

    // e.SetArgs(eargs);

    // if (client->online() && client->isOpen()) {
        // client->send(e);
    // }
    sockets.erase(client);
    delete client;
}

void CommServer::removeSocket(CommSocket * client)
{
    removeSocket(client,"You caused exception. Connection closed");
}

#if 0
#define MAXLINE 4096

void CommServer::metaserverKeepalive()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(SKEEP_ALIVE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size,0, (sockaddr *)&meta_sa, sizeof(meta_sa));
}

void CommServer::metaserverReply()
{
    char                mesg[MAXLINE];
    char               *mesg_ptr;
    uint32_t            handshake = 0, command = 0;
    struct sockaddr	addr;
    socklen_t           addrlen;
    unsigned int        packet_size;

    if (recvfrom(metaFd, mesg, MAXLINE, 0, &addr, &addrlen) < 0) {
        log(WARNING, "WARNING: No reply from metaserver");
        return;
    }
    mesg_ptr = unpack_uint32(&command, mesg);

    if(command == HANDSHAKE)
    {
        mesg_ptr = unpack_uint32(&handshake, mesg_ptr);
        debug(std::cout << "Server contacted successfully." << std::endl << std::flush;);

        packet_size = 0;
        mesg_ptr = pack_uint32(SERVERSHAKE, mesg, &packet_size);
        mesg_ptr = pack_uint32(handshake, mesg_ptr, &packet_size);

        sendto(metaFd,mesg,packet_size,0,(sockaddr*)&meta_sa,sizeof(meta_sa));
    }

}

void CommServer::metaserverTerminate()
{
    char         mesg[MAXLINE];
    unsigned int packet_size = 0;

    pack_uint32(TERMINATE, mesg, &packet_size);
    sendto(metaFd,mesg,packet_size, 0, (sockaddr *)&meta_sa, sizeof(meta_sa));
}
#endif
