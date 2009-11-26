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

#include "ClientConnection.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

static bool debug_flag = false;

ClientConnection::ClientConnection() : serialNo(512)
{
}

ClientConnection::~ClientConnection()
{
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
    if (op->isDefaultFrom()) {
        if (op->isDefaultArgs() || op->getArgs().empty()) {
            std::cerr << "WARNING: Malformed account from server" << std::endl << std::flush;
        } else {
            const Root & ac = op->getArgs().front();
            reply = ac;
            // const std::string & acid = reply["id"].asString();
            // objects[acid] = new ClientAccount(acid, *this);
        }
    } else {
        operation(op);
    }
}

int ClientConnection::read() {
    if (m_ios != 0 && m_ios->is_open()) {
        m_codec->poll();
        return 0;
    } else {
        return -1;
    }
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
      if (poll(1) != 0) {
          return -1;
      }
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
    AtlasStreamClient::send(op);
}

int ClientConnection::poll(int timeOut, int msec)
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
