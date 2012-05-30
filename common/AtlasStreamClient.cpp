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
#include "common/ClientTask.h"
#include "common/system.h"

#include "common/debug.h"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Net/Stream.h>

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <skstream/skstream.h>
#include <skstream/skstream_unix.h>

#include <cstdio>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::RootOperation;

void AtlasStreamClient::output(const Element & item, int depth) const
{
    output_element(std::cout, item, depth);
}

/// \brief Function call from the base class when an object arrives from the
/// server
///
/// @param obj Object that has arrived from the server
void AtlasStreamClient::objectArrived(const Root & obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        std::cerr << "ERROR: Non op object received from server"
                  << std::endl << std::flush;;
        if (!obj->isDefaultParents() && !obj->getParents().empty()) {
            std::cerr << "NOTICE: Unexpected object has parent "
                      << obj->getParents().front()
                      << std::endl << std::flush;
        }
        if (!obj->isDefaultObjtype()) {
            std::cerr << "NOTICE: Unexpected object has objtype "
                      << obj->getObjtype()
                      << std::endl << std::flush;
        }
        return;
    }

    operation(op);
}

void AtlasStreamClient::operation(const RootOperation & op)
{
    if (m_currentTask != 0) {
        OpVector res;
        m_currentTask->operation(op, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }

        if (m_currentTask->isComplete()) {
            delete m_currentTask;
            m_currentTask = 0;
        }
    }

    switch (op->getClassNo()) {
        case Atlas::Objects::Operation::APPEARANCE_NO:
            appearanceArrived(op);
            break;
        case Atlas::Objects::Operation::DISAPPEARANCE_NO:
            disappearanceArrived(op);
            break;
        case Atlas::Objects::Operation::INFO_NO:
            infoArrived(op);
            break;
        case Atlas::Objects::Operation::ERROR_NO:
            errorArrived(op);
            break;
        case Atlas::Objects::Operation::SIGHT_NO:
            sightArrived(op);
            break;
        case Atlas::Objects::Operation::SOUND_NO:
            soundArrived(op);
            break;
        default:
            break;
    }
}

void AtlasStreamClient::infoArrived(const RootOperation & op)
{
    reply_flag = true;
    if (!op->isDefaultFrom()) {
        return;
    }
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        std::cerr << "WARNING: Malformed account from server" << std::endl << std::flush;
        return;
    }
    if (op->isDefaultRefno()) {
        return;
    }
    if (op->getRefno() != serialNo) {
        return;
    }
    m_infoReply = op->getArgs().front();
}

void AtlasStreamClient::appearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation & op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation & op)
{
}

void AtlasStreamClient::loginSuccess(const Atlas::Objects::Root & arg)
{
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AtlasStreamClient::errorArrived(const RootOperation & op)
{
    reply_flag = true;
    error_flag = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }
    const Root & arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        m_errorMessage = message_attr.String();
    }
}

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_fd(-1), m_encoder(0),
                                         m_codec(0), m_ios(0), m_currentTask(0),
                                         m_spacing(2)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
    delete m_encoder;
    delete m_codec;
    delete m_ios;
}

void AtlasStreamClient::send(const RootOperation & op)
{
    if (m_encoder == 0) {
        return;
    }

    // There is no way this should ever be null if m_encoder is
    // not null, as m_encoder is set following negotiation, and that
    // can't happen without a socket stream
    assert(m_ios != 0);

    reply_flag = false;
    error_flag = false;
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

    socket_linger(m_fd, 10);

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

    socket_client_send_credentials(m_fd);

    socket_linger(m_fd, 10);

    return negotiate();
#else // HAVE_SYS_UN_H
    return -1;
#endif // HAVE_SYS_UN_H
}

int AtlasStreamClient::negotiate()
{
    assert(m_ios != 0);

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
    l->setSerialno(newSerialNo());
 
    send(l);

    return waitForLoginResponse();
}

int AtlasStreamClient::create(const std::string & type,
                              const std::string & username,
                              const std::string & password)
{
    m_username = username;

    Create c;
    Anonymous account;

    account->setAttr("username", username);
    account->setAttr("password", password);
    account->setParents(std::list<std::string>(1, type));

    c->setArgs1(account);
    c->setSerialno(newSerialNo());

    send(c);

    return waitForLoginResponse();
}

int AtlasStreamClient::waitForLoginResponse()
{
    for (int i = 0; i < 10; ++i) {
       if (poll(0, 100000) != 0) {
           return -1;
       }
       if (reply_flag && !error_flag) {
           if (m_infoReply->isDefaultId()) {
              std::cerr << "Malformed reply" << std::endl << std::flush;
           } else {
               if (!m_infoReply->getParents().empty()) {
                   accountType = m_infoReply->getParents().front();
               }
               loginSuccess(m_infoReply);
               return 0;
           }
           reply_flag = false;
       }
    }

    return -1;
}

int AtlasStreamClient::poll(int timeOut, int msec)
{
    if (m_fd < 0) {
        return -1;
    }

    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(m_fd, &infds);

    tv.tv_sec = timeOut;
    tv.tv_usec = msec;

    int retval = select(m_fd+1, &infds, NULL, NULL, &tv);

    if (retval < 1) {
        return retval;
    }

    if (FD_ISSET(m_fd, &infds)) {
        if (m_ios->peek() == -1) {
            std::cerr << "Server disconnected" << std::endl << std::flush;
            return -1;
        } else {
            m_codec->poll();
        }
    }
    return 0;
}

int AtlasStreamClient::runTask(ClientTask * task, const std::string & arg)
{
    assert(task != 0);

    if (m_currentTask != 0) {
        std::cout << "Busy" << std::endl << std::flush;
        return -1;
    }

    m_currentTask = task;

    OpVector res;

    m_currentTask->setup(arg, res);

    if (m_currentTask->isComplete()) {
        delete task;
        m_currentTask = 0;
        return -1;
    }

    OpVector::const_iterator Iend = res.end();
    for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        send(*I);
    }
    return 0;
}

int AtlasStreamClient::endTask()
{
    if (m_currentTask == 0) {
        return -1;
    }
    delete m_currentTask;
    m_currentTask = 0;
    return 0;
}
