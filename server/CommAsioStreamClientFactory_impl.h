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

#ifndef COMMASIOSTREAMCLIENTFACTORY_IMPL_H_
#define COMMASIOSTREAMCLIENTFACTORY_IMPL_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "CommAsioStreamClientFactory.h"

template<typename ClientT, typename ConnectionT>
CommAsioStreamClientFactory<ClientT, ConnectionT>::CommAsioStreamClientFactory(
        ServerRouting& server) :
        mServer(server)
{
}

template<typename ClientT, typename ConnectionT>
CommAsioStreamClientFactory<ClientT, ConnectionT>::~CommAsioStreamClientFactory()
{
}

template<typename ClientT, typename ConnectionT>
std::shared_ptr<ClientT> CommAsioStreamClientFactory<ClientT, ConnectionT>::createClient(
        boost::asio::io_service& io_service)
{
    return std::make_shared < ClientT > (mServer.getName(), io_service);
}

template<typename ClientT, typename ConnectionT>
void CommAsioStreamClientFactory<ClientT, ConnectionT>::startClient(
        ClientT& client)
{
    std::string connection_id;
    long c_iid = newId(connection_id);
    client.startAccept(
            new ConnectionT(client, mServer, "", connection_id, c_iid));
}

#endif /* COMMASIOSTREAMCLIENTFACTORY_IMPL_H_ */

