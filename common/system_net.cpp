// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <cassert>

extern "C" {
#ifdef HAVE_SYS_UTSNAME_H
    #include <sys/utsname.h>
#endif // HAVE_SYS_UTSNAME_H
    #include <sys/types.h>
}

#ifdef HAVE_WINSOCK_H
#include <winsock2.h>
#endif // HAVE_WINSOCK_H

#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif // HAVE_SYS_UN_H

static const bool debug_flag = false;

const std::string get_hostname()
{
#ifndef HAVE_UNAME
    char hostname_buf[256];

    if (gethostname(hostname_buf, 256) != 0) {
        return "UNKNOWN";
    }
    return std::string(hostname_buf);
#else // HAVE_UNAME
    struct utsname host_ident;
    if (uname(&host_ident) != 0) {
        return "UNKNOWN";
    }
    return std::string(host_ident.nodename);
#endif // HAVE_UNAME
}

int socket_linger(SOCKET_TYPE fd, int time)
{
    struct linger {
        int   l_onoff;
        int   l_linger;
    } listenLinger = { 1, time };
    ::setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&listenLinger,
                                              sizeof(listenLinger));
    return 0;
}

int socket_client_send_credentials(int fd)
{
#ifdef HAVE_SYS_UN_H
#ifndef __APPLE__
#ifndef SO_PEERCRED
    // Prove to the server that we are real.

    unsigned char buf[1];

    buf[0] = '\0';

    struct iovec vec[1];

    vec[0].iov_base = buf;
    vec[0].iov_len = 1;

    struct msghdr auth_message;

    auth_message.msg_iov = vec;
    auth_message.msg_iovlen = 1;
    auth_message.msg_name = 0;
    auth_message.msg_namelen = 0;
    auth_message.msg_controllen = CMSG_SPACE(sizeof(struct ucred));
    unsigned char * mcb = new unsigned char[auth_message.msg_controllen];
    auth_message.msg_control = mcb;
    auth_message.msg_flags = 0;

    struct cmsghdr * control = CMSG_FIRSTHDR(&auth_message);
    control->cmsg_len = CMSG_LEN(sizeof(struct ucred));
    control->cmsg_level = SOL_SOCKET;
    control->cmsg_type = SCM_CREDENTIALS;
    struct ucred * creds = (struct ucred *)CMSG_DATA(control);
    creds->pid = ::getpid();
    creds->uid = ::getuid();
    creds->gid = ::getgid();

    int serr = sendmsg(m_fd, &auth_message, 0);

    delete [] mcb;

    if (serr < 0) {
        perror("sendmsg");
    }

    // Done proving we are real.
#endif // SO_PEERCRED
    return 0;
#endif // __APPLE__
#else // HAVE_SYS_UN_H
    return -1;
#endif // HAVE_SYS_UN_H
}

