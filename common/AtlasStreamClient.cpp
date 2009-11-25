// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "common/AtlasStreamClient.h"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Net/Stream.h>

#include <skstream/skstream.h>
#include <skstream/skstream_unix.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::RootOperation;

int AtlasStreamClient::authenticateLocal()
{
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

    int serr = sendmsg(m_fd, &auth_message, 0);

    if (serr < 0) {
        perror("sendmsg");
    }

    // Done proving we are real.
    return 0;
}

int AtlasStreamClient::linger()
{
    struct linger {
        int   l_onoff;
        int   l_linger;
    } listenLinger = { 1, 10 };
    ::setsockopt(m_fd, SOL_SOCKET, SO_LINGER, (char *)&listenLinger,
                                                   sizeof(listenLinger));
    // Ensure the address can be reused once we are done with it.
    return 0;
}

void AtlasStreamClient::output(const Element & item, int depth) const
{
    switch (item.getType()) {
        case Element::TYPE_INT:
            std::cout << item.Int();
            break;
        case Element::TYPE_FLOAT:
            std::cout << item.Float();
            break;
        case Element::TYPE_STRING:
            std::cout << "\"" << item.String() << "\"";
            break;
        case Element::TYPE_LIST:
            {
                std::cout << "[ ";
                ListType::const_iterator I = item.List().begin();
                ListType::const_iterator Iend = item.List().end();
                for(; I != Iend; ++I) {
                    output(*I, depth);
                    std::cout << " ";
                }
                std::cout << "]";
            }
            break;
        case Element::TYPE_MAP:
            {
                std::cout << "{" << std::endl << std::flush;
                MapType::const_iterator I = item.Map().begin();
                MapType::const_iterator Iend = item.Map().end();
                for(; I != Iend; ++I) {
                    std::cout << std::string((depth + 1) * 4, ' ') << I->first << ": ";
                    output(I->second, depth + 1);
                    std::cout << std::endl;
                }
                std::cout << std::string(depth * 4, ' ') << "}";
            }
            break;
        default:
            std::cout << "(\?\?\?)";
            break;
    }
}

AtlasStreamClient::AtlasStreamClient() : m_encoder(0), m_codec(0), m_ios(0)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
    if (m_encoder != NULL) {
        delete m_encoder;
    }
    if (m_codec != NULL) {
        delete m_codec;
    }
    if (m_ios != NULL) {
        delete m_ios;
    }
}

void AtlasStreamClient::send(const RootOperation & op)
{
    m_encoder->streamObjectsMessage(op);
    (*m_ios) << std::flush;
}

int AtlasStreamClient::connect(const std::string & host, int port)
{
    m_ios = new tcp_socket_stream(host, port);
    if (!m_ios->is_open()) {
        return -1;
    }
    m_fd = m_ios->getSocket();

    linger();

    return negotiate();

}

int AtlasStreamClient::connectLocal(const std::string & filename)
{
#ifdef HAVE_SYS_UN_H
    m_ios = new unix_socket_stream(filename);
    if (!m_ios->is_open()) {
        return -1;
    }

    m_fd = m_ios->getSocket();

    authenticateLocal();

    linger();

    return negotiate();
#else // HAVE_SYS_UN_H
    return -1;
#endif // HAVE_SYS_UN_H
}

int AtlasStreamClient::negotiate()
{
    Atlas::Net::StreamConnect conn("cyphesis_aiclient", *m_ios);

    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      conn.poll();
    }
  
    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return -1;
    }

    m_codec = conn.getCodec(*this);

    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);

    m_codec->streamBegin();

    return 0;
}

int AtlasStreamClient::login(const std::string & username,
                             const std::string & password)
{
    m_username = username;

    Login l;
    Anonymous account;
 
    account->setAttr("username", username);
    account->setAttr("password", password);
 
    l->setArgs1(account);
 
    send(l);

    return 0;
}

int AtlasStreamClient::create(const std::string & username,
                               const std::string & password)
{
    m_username = username;

    Create c;
    Anonymous account;

    account->setAttr("username", username);
    account->setAttr("password", password);

    c->setArgs1(account);

    send(c);

    return 0;
}
