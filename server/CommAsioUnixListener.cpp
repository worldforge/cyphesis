/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommAsioUnixListener.h"
#include "TrustedConnection.h"
#include "common/id.h"

CommAsioUnixListener::CommAsioUnixListener(CommServer& commServer,
        ServerRouting& server, boost::asio::io_service& ioService,
        const std::string& socketPath) :
        mCommServer(commServer), mServer(server), mAcceptor(ioService,
                boost::asio::local::stream_protocol::endpoint(socketPath)), mSocket(
                ioService)
{
    startAccept();
}

CommAsioUnixListener::~CommAsioUnixListener()
{
}

void CommAsioUnixListener::startAccept()
{
    mAcceptor.async_accept(mSocket,
            [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::string connection_id;
                    long c_iid = newId(connection_id);

                    auto client = std::make_shared<CommAsioAdminClient>(mCommServer, "server", std::move(mSocket));
                    client->setup(new TrustedConnection(*client,
                                    mServer,
                                    "",
                                    connection_id, c_iid));
                }

                startAccept();
            });
}

