// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommUnixListener.h"

#include "CommLocalClient.h"
#include "CommServer.h"

#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"

#include <iostream>

#include <cstdio>

#include <sys/socket.h>
#include <sys/un.h>

static const bool debug_flag = false;

CommUnixListener::~CommUnixListener()
{
    if (bound) {
        std::string sock_path = var_directory + "/cyphesis.sock";
        unlink(sock_path.c_str());
    }
}


int CommUnixListener::getFd() const
{
    return listenFd;
}

bool CommUnixListener::eof()
{
    return false;
}
bool CommUnixListener::isOpen() const
{
    return true;
}

bool CommUnixListener::read()
{
    accept();
    return false;
}

#define UNIX_PATH_MAX 108

bool CommUnixListener::setup()
{
    // Nasty low level socket code to set up listen socket. This should be
    // replaced with a socket class library.
    struct sockaddr_un sun;

    listenFd = ::socket(PF_UNIX, SOCK_STREAM, 0);
    if (listenFd < 0) {
        perror("socket");
        return false;
    }
    // int flag = 1;
    // ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sun.sun_family = AF_UNIX;
    std::string sock_path = var_directory + "/cyphesis.sock";
    strncpy(sun.sun_path, sock_path.c_str(), UNIX_PATH_MAX);
    sun.sun_path[UNIX_PATH_MAX - 1] = '\0';
    if (::bind(listenFd, (struct sockaddr *)&sun, sizeof(sun)) < 0) {
        log(ERROR, "Unable to open unix listen socket");
        perror("bind");
        ::close(listenFd);
        return false;
    }
    bound = true;
    if (::listen(listenFd, 5) < 0) {
        log(ERROR, "Unable to listen to unix socket");
        perror("bind");
        ::close(listenFd);
        return false;
    }
    return true;
}

bool CommUnixListener::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_un sun;
    unsigned int addr_len = sizeof(sun);

    sun.sun_family = AF_UNIX;

    debug(std::cout << "Local accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(listenFd, (struct sockaddr *)&sun, &addr_len);

    if (asockfd < 0) {
        return false;
    }
    debug(std::cout << "Local accepted" << std::endl << std::flush;);
    CommLocalClient * newcli = new CommLocalClient(commServer, asockfd);

    newcli->setup();

    // Add this new client to the list.
    commServer.add(newcli);

    return true;
}
