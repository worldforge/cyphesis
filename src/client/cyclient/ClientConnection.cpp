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


#include "ClientConnection.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::RootOperation;

static bool debug_flag = false;

ClientConnection::ClientConnection(boost::asio::io_context& io_context, Atlas::Objects::Factories& factories)
: AtlasStreamClient(io_context, factories)

{
}

ClientConnection::~ClientConnection() = default;

void ClientConnection::operation(const RootOperation & op)
{
    debug_print("A " << op->getParent() << " op from server!")

    reply_flag = true;
    operationQueue.push_back(op);

    AtlasStreamClient::operation(op);
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

int ClientConnection::wait()
// Waits for response from server. Used when we are expecting a login response
// Return whether or not an error occured
{
   error_flag = false;
   reply_flag = false;
   while (!reply_flag) {
      if (poll(std::chrono::seconds(1)) != 0) {
          return -1;
      }
   }
   return error_flag ? -1 : 0;
}

int ClientConnection::sendAndWaitReply(const Operation & op, OpVector & res)
{
    long no = newSerialNo();
    op->setSerialno(no);
    send(op);
    debug_print("Waiting for reply to " << op->getParent());
    while (true) {
        if (pending()) {
            Operation input = pop();
            assert(input.isValid());
            if (input.isValid()) {
                if (input->getRefno() == no) {
                    debug_print("Got reply")
                    res.push_back(input);
                    return 0;
                } else {
                    debug_print("Not reply")
                }
            } else {
                debug_print("Not op")
            }
        } else if (wait() != 0) {
            return -1;
        }
    }
}

RootOperation ClientConnection::pop()
{
    poll(std::chrono::seconds::zero());
    if (operationQueue.empty()) {
        return RootOperation(nullptr);
    }
    RootOperation op = operationQueue.front();
    operationQueue.pop_front();
    return op;
}

bool ClientConnection::pending()
{
    return !operationQueue.empty();
}
