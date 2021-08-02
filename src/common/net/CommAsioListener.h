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

#ifndef COMMASIOLISTENER_H_
#define COMMASIOLISTENER_H_

#include "common/asio.h"

#include <functional>

template<typename ProtocolT, typename ClientT>
class CommAsioListener
{
    public:
        CommAsioListener(std::function<std::shared_ptr<ClientT>()> clientCreator,
                         std::function<void(ClientT&)> clientStarter,
                         std::string serverName,
                         boost::asio::io_context& ioService,
                         const typename ProtocolT::endpoint& endpoint);

        virtual ~CommAsioListener();

    protected:
        std::function<std::shared_ptr<ClientT>()> mClientCreator;
        std::function<void(ClientT&)> mClientStarter;
        const std::string mServerName;
        Atlas::Objects::Factories* mFactories;

        typename ProtocolT::acceptor mAcceptor;

        void startAccept();
};

#endif /* COMMASIOLISTENER_H_ */
