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

#include "InterServerConnection.h"

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

InterServerConnection::InterServerConnection()
{
}

InterServerConnection::~InterServerConnection()
{
}

void InterServerConnection::operation(const RootOperation & op)
{
    debug(std::cout << "A " << op->getParents().front() << " op from server!" << std::endl << std::flush;);

    reply_flag = true;
    operationQueue.push_back(op);

    AtlasStreamClient::operation(op);
}

// Waits for response from server. Used when we are expecting a response
// Return whether or not an error occured
int InterServerConnection::wait()
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

RootOperation InterServerConnection::pop()
{
    poll();
    if (operationQueue.empty()) {
        return RootOperation(0);
    }
    RootOperation op = operationQueue.front();
    operationQueue.pop_front();
    return op;
}

bool InterServerConnection::pending()
{
    return !operationQueue.empty();
}
