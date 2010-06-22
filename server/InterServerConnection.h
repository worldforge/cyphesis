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

#ifndef SERVER_INTER_SERVER_CONNECTION_H
#define SERVER_INTER_SERVER_CONNECTION_H

#include "common/AtlasStreamClient.h"

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Root.h>

#include <skstream/skstream.h>

#include <deque>

/// \brief Class to handle socket connection between cyphesis instances
class InterServerConnection : public AtlasStreamClient {
  protected:
    /// \brief Store for operations arrived from the server
    std::deque<Atlas::Objects::Operation::RootOperation> operationQueue;

    virtual void operation(const Atlas::Objects::Operation::RootOperation&);

  public:
    InterServerConnection();
    ~InterServerConnection();

    int wait();
	std::string getAccountId() { return accountId; }

    Atlas::Objects::Operation::RootOperation pop();
    bool pending();
};

#endif // SERVER_INTER_SERVER_CONNECTION_H
