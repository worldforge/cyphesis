// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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


#ifndef SERVER_TRUSTED_CONNECTION_H
#define SERVER_TRUSTED_CONNECTION_H

#include "Connection.h"

/// \brief This class represents a connection from a client that is inherently
/// trusted, and can thus login without a password
///
/// This allows us to allow clients to login automatically without needing to
/// store the admin password in plain text anywhere. The client might be
/// trusted because it has connected with a known certificate of some kind,
/// or because it has connected over a unix domain socket.
class TrustedConnection : public Connection
{
    protected:
        std::unique_ptr<Account> newAccount(const std::string& type,
                            const std::string& username,
                            const std::string& passwd,
                            RouterId id) override;

    public:
        TrustedConnection(CommSocket& client,
                          ServerRouting& svr,
                          const std::string& addr,
                          RouterId id);

        ~TrustedConnection() override;
};

#endif // SERVER_TRUSTED_CONNECTION_H
