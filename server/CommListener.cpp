// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "CommListener.h"

#include "CommClient.h"
#include "CommServer.h"

#include "common/debug.h"

#include <iostream>

#include <cstdio>

#include <sys/socket.h>

static const bool debug_flag = false;

int CommListener::getFd() const
{
    return listenFd;
}

bool CommListener::eof()
{
    return false;
}
bool CommListener::isOpen() const
{
    return true;
}

bool CommListener::read()
{
    accept();
    return false;
}

bool CommListener::setup(int port)
{
    // Nasty low level socket code to set up listen socket. This should be
    // replaced with a socket class library.
    struct sockaddr_in sin;

    listenPort = port;
    listenFd = ::socket(PF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
	perror("socket");
        return false;
    }
    int flag = 1;
    ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = 0L;
    if (::bind(listenFd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	perror("bind");
        ::close(listenFd);
        return false;
    }
    ::listen(listenFd, 5);
    return true;
}

bool CommListener::accept()
{
    // Low level socket code to accept a new client connection, and create
    // the associated commclient object.
    struct sockaddr_in sin;
    unsigned int addr_len = sizeof(sin);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(listenPort);
    sin.sin_addr.s_addr = 0L;

    debug(std::cout << "Accepting.." << std::endl << std::flush;);
    int asockfd = ::accept(listenFd, (struct sockaddr *)&sin, &addr_len);

    if (asockfd < 0) {
        return false;
    }
    debug(std::cout << "Accepted" << std::endl << std::flush;);
    CommClient * newcli = new CommClient(commServer, asockfd, sin.sin_port);

    newcli->setup();

    // Add this new client to the list.
    commServer.add(newcli);

    return true;
}
