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

#ifndef COMMASIOLISTENER_IMPL_H_
#define COMMASIOLISTENER_IMPL_H_

#include "CommAsioListener.h"
#include "common/CommAsioClient_impl.h"

template<class ProtocolT, typename ClientT>
CommAsioListener<ProtocolT, ClientT>::CommAsioListener(
    std::function<void(ClientT&)> clientStarter,
    std::string serverName, boost::asio::io_context& ioService,
    const typename ProtocolT::endpoint& endpoint)
    : mClientStarter(clientStarter), mServerName(std::move(serverName)), mAcceptor(ioService, endpoint)
{
    startAccept();
}

template<class ProtocolT, typename ClientT>
CommAsioListener<ProtocolT, ClientT>::~CommAsioListener()
{
    mAcceptor.cancel();
}

template<class ProtocolT, typename ClientT>
void CommAsioListener<ProtocolT, ClientT>::startAccept()
{
#if BOOST_VERSION < 106600
    auto client = std::make_shared<ClientT>(mServerName, mAcceptor.get_io_service());
#else
    auto client = std::make_shared<ClientT>(mServerName, mAcceptor.get_executor().context());
#endif
    mAcceptor.async_accept(client->getSocket(),
                           [this, client](boost::system::error_code ec) {
                               if (!ec) {
                                   mClientStarter(*client);
                                   this->startAccept();
                               }
                           });
}

#endif /* COMMASIOLISTENER_IMPL_H_ */
