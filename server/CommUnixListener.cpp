// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommUnixListener.h"

#include "CommClientFactory.h"
#include "CommServer.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/system.h"
#include "common/globals.h"

#include <iostream>

#include <unistd.h>

#include <cassert>

#ifdef HAVE_SYS_UN_H

#include <sys/un.h>

#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif // HAVE_SYS_UCRED_H

static const bool debug_flag = false;

/// \brief Constructor unix listen socket object.
///
/// @param svr Reference to the object that manages all socket communication.
CommUnixListener::CommUnixListener(CommServer & svr, CommClientKit & kit) :
                  CommStreamListener(svr, kit)
{
    m_listener = &m_unixListener;
}

CommUnixListener::~CommUnixListener()
{
    if (m_unixListener.is_open()) {
        ::unlink(m_path.c_str());
    }
}

/// \brief Create and bind the listen socket.
///
/// If an initial attempt fails, then this function attempts to unlink the
/// unix socket path, and tries again. This method is deliberatly called
/// _after_ attempting to open the TCP listen socket, so that this
/// will never be called in an instance of cyphesis already exists.
int CommUnixListener::setup(const std::string & name)
{
    m_path = name;

    m_unixListener.open(m_path);
    bool bound = m_unixListener.is_open();

    if (!bound) {
        ::unlink(m_path.c_str());

        m_unixListener.open(m_path);
        bound = m_unixListener.is_open();
    }

    return bound ? 0 : 1;
}

/// \brief Accept a new connect to the listen socket.
int CommUnixListener::accept()
{
    debug(std::cout << "Local accepting.." << std::endl << std::flush;);
    int fd = m_unixListener.accept();

    if (fd < 0) {
        return -1;
    }
    debug(std::cout << "Local accepted" << std::endl << std::flush;);

#ifdef HAVE_SYS_UN_H
#ifdef __APPLE__
    struct xucred creds;
    creds.cr_version = 0;
    creds.cr_uid = 24;
    socklen_t cred_len = sizeof(creds);
    int ret = ::getsockopt(fd, SOL_SOCKET, LOCAL_PEERCRED, &creds, &cred_len);
    if (ret != 0 || cred_len != sizeof(creds)) {
        log(WARNING, "Unable to get unix credentials.");
        if (ret != 0) {
            logSysError(ERROR);
        }
    }
#else // __APPLE__
    // Start getting data from the client about who it really is

    int flagon = 1;
    if (::setsockopt(fd, SOL_SOCKET, SO_PASSCRED,
                     &flagon, sizeof(int)) < 0) {
        log(ERROR, "Unable to enable unix credentials.");
        logSysError(ERROR);
    } else {

        char data_buf[1], control_buf[1024];

        struct iovec vec;
        vec.iov_base        = data_buf;
        vec.iov_len         = sizeof(data_buf);

        struct msghdr auth_message;
        auth_message.msg_name        = NULL;
        auth_message.msg_namelen     = 0;
        auth_message.msg_iov         = &vec;
        auth_message.msg_iovlen      = 1;
        auth_message.msg_control     = control_buf;
        auth_message.msg_controllen  = sizeof(control_buf);
        auth_message.msg_flags       = 0;

        if (recvmsg(fd, &auth_message, 0) == -1) {
            log(WARNING, "Credentials recieve failed");
        }

        struct cmsghdr * cmsg = 0;
        struct ucred * creds = 0;
        for (cmsg = CMSG_FIRSTHDR(&auth_message);
             cmsg != NULL;
             cmsg = CMSG_NXTHDR(&auth_message, cmsg)) {
            if (cmsg->cmsg_level == SOL_SOCKET &&
                cmsg->cmsg_type == SCM_CREDENTIALS) {
                creds = (struct ucred *)CMSG_DATA(cmsg);
                break;
            }
        }

        if (creds == 0) {
            log(ERROR, "Unix client connected but did not give credentials.");
        }
    }
#endif // __APPLE__
#endif // HAVE_SYS_UN_H

    // Got client auth data

    return create(fd, "local");
}

#endif // HAVE_SYS_UN_H
