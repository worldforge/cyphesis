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

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "ClientConnection.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <Atlas/Codecs/XML.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cstdio>

#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif // HAVE_SYS_UN_H

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

static bool debug_flag = false;

ClientConnection::ClientConnection() :
    client_fd(-1), encoder(NULL), serialNo(512)
{
}

ClientConnection::~ClientConnection()
{
    if (encoder != NULL) {
        delete encoder;
    }
}

void ClientConnection::operation(const RootOperation & op)
{
#if 0
    const std::string & from = op->getFrom();
    if (from.empty()) {
        std::cerr << "ERROR: Operation with no destination" << std::endl << std::flush;
        return;
    }
    dict_t::const_iterator I = objects.find(from);
    if (I == objects.end()) {
        std::cerr << "ERROR: Operation with invalid destination" << std::endl << std::flush;
        return;
    }
    OpVector res = I->second->message(op);
    OpVector::const_iterator Jend = res.end();
    fora (OpVector::const_iterator J = res.begin(); J != Jend; ++J) {
        (*J)->setFrom(I->first);
        send(*(*J));
    }
#endif
}

void ClientConnection::objectArrived(const Atlas::Objects::Root & obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        const std::list<std::string> & parents = obj->getParents();
        if (parents.empty()) {
            log(ERROR, String::compose("Object of type \"%1\" with no parent arrived from server", obj->getObjtype()));
        } else {
            log(ERROR, String::compose("Object of type \"%1\" with parent \"%2\" arrived from server", obj->getObjtype(), obj->getParents().front()));
        }
        return;
    }
    debug(std::cout << "A " << op->getParents().front() << " op from server!" << std::endl << std::flush;);

    reply_flag = true;
    operationQueue.push_back(op);

    if (op->getClassNo() == Atlas::Objects::Operation::ERROR_NO) {
        errorArrived(op);
    } else if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op);
    }
}

void ClientConnection::errorArrived(const RootOperation & op)
{
    debug(std::cout << "ERROR" << std::endl << std::flush;);
    error_flag = true;
}

void ClientConnection::infoArrived(const RootOperation & op)
{
    debug(std::cout << "INFO" << std::endl << std::flush;);
    const std::string & from = op->getFrom();
    if (from.empty()) {
        try {
            const Root & ac = op->getArgs().front();
            reply = ac;
            // const std::string & acid = reply["id"].asString();
            // objects[acid] = new ClientAccount(acid, *this);
        }
        catch (...) {
            std::cerr << "WARNING: Malformed account from server" << std::endl << std::flush;
        }
    } else {
        operation(op);
    }
}

int ClientConnection::read() {
    if (ios.is_open()) {
        codec->poll();
        return 0;
    } else {
        return -1;
    }
}

int ClientConnection::connectLocal(const std::string & sockname)
{
#ifdef HAVE_SYS_UN_H
    debug(std::cout << "Attempting local connect." << std::endl << std::flush;);

    struct sockaddr_un sun;
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, sockname.c_str(), sizeof(sun.sun_path));

    int fd = ::socket(PF_UNIX, SOCK_STREAM, 0);

    if (0 != ::connect(fd, (struct sockaddr *)&sun, sizeof(sun))) {
        debug(std::cout << "Local connect refused" << std::endl << std::flush;);
        return -1;
    }

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
    auth_message.msg_control = new unsigned char[auth_message.msg_controllen];
    auth_message.msg_flags = 0;

    struct cmsghdr * control = CMSG_FIRSTHDR(&auth_message);
    control->cmsg_len = CMSG_LEN(sizeof(struct ucred));
    control->cmsg_level = SOL_SOCKET;
    control->cmsg_type = SCM_CREDENTIALS;
    struct ucred * creds = (struct ucred *)CMSG_DATA(control);
    creds->pid = ::getpid();
    creds->uid = ::getuid();
    creds->gid = ::getgid();

    int serr = sendmsg(fd, &auth_message, 0);

    if (serr > 0) {
        std::cout << "SENT:" << serr << std::endl << std::flush;
    } else {
        perror("sendmsg");
    }

    // Done proving we are real.

    ios.setSocket(fd);
    if (!ios.is_open()) {
        std::cerr << "ERROR: For some reason " << sockname << " not open."
                  << std::endl << std::flush;
        return -1;
    }

    client_fd = ios.getSocket();

    linger();
    int ret = negotiate();

    if (ret == -1) {
        ios.close();
    }
    return ret;
#else // HAVE_SYS_UN_H
    return -1;
#endif // HAVE_SYS_UN_H
}

int ClientConnection::connect(const std::string & server, int port)
{
    debug(std::cout << "Connecting to " << server << std::endl << std::flush;);

    ios.open(server, port);
    if (!ios.is_open()) {
        debug(std::cerr << "ERROR: Could not connect to " << server << "."
                        << std::endl << std::flush;);
        return -1;
    }

    client_fd = ios.getSocket();

    linger();
    return negotiate();
}

int ClientConnection::linger()
{
    struct linger {
        int   l_onoff;
        int   l_linger;
    } listenLinger = { 1, 10 };
    ::setsockopt(client_fd, SOL_SOCKET, SO_LINGER, (char *)&listenLinger,
                                                   sizeof(listenLinger));
    // Ensure the address can be reused once we are done with it.
    return 0;
}

int ClientConnection::negotiate()
{
    Atlas::Net::StreamConnect conn("cyphesis_aiclient", ios);

    debug(std::cout << "Negotiating... " << std::flush;);
    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      conn.poll();
    }
    debug(std::cout << "done" << std::endl;);
  
    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return -1;
    }

    codec = conn.getCodec(*this);

    encoder = new Atlas::Objects::ObjectsEncoder(*codec);

    codec->streamBegin();

    return 0;
}

void ClientConnection::login(const std::string & account,
                             const std::string & password)
{
    Atlas::Objects::Operation::Login l;
    Anonymous login_arg;
    login_arg->setAttr("username", account);
    login_arg->setAttr("password", password);

    l->setArgs1(login_arg);

    reply_flag = false;
    error_flag = false;
    send(l);
}

void ClientConnection::create(const std::string & account,
                              const std::string & password)
{
    Atlas::Objects::Operation::Create c;
    Anonymous create_arg;
    create_arg->setAttr("username", account);
    create_arg->setAttr("password", password);

    c->setArgs1(create_arg);

    reply_flag = false;
    error_flag = false;
    send(c);
}

int ClientConnection::wait()
// Waits for response from server. Used when we are expecting a login response
// Return whether or not an error occured
{
   error_flag = false;
   reply_flag = false;
   while (!reply_flag) {
      poll(1);
   }
   return error_flag ? -1 : 0;
}

void ClientConnection::send(const RootOperation & op)
{
    /* debug(Atlas::Codecs::XML c((std::iostream&)std::cout, (Atlas::Bridge*)this);
          Atlas::Objects::Encoder enc(&c);
          enc.streamMessage(&op);
          std::cout << std::endl << std::flush;); */

    op->setSerialno(++serialNo);
    encoder->streamObjectsMessage(op);
    ios << std::flush;
}

void ClientConnection::poll(int timeOut)
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(client_fd, &infds);

    tv.tv_sec = timeOut;
    tv.tv_usec = 0;

    int retval = select(client_fd+1, &infds, NULL, NULL, &tv);

    if (retval < 1) {
        return;
    }

    if (FD_ISSET(client_fd, &infds)) {
        if (ios.peek() == -1) {
            std::cerr << "Server disconnected" << std::endl << std::flush;
            error_flag = true;
            reply_flag = true;
        } else {
            codec->poll();
        }
    }
}

RootOperation ClientConnection::pop()
{
    poll();
    if (operationQueue.empty()) {
        return RootOperation(0);
    }
    RootOperation op = operationQueue.front();
    operationQueue.pop_front();
    return op;
}

bool ClientConnection::pending()
{
    return !operationQueue.empty();
}
