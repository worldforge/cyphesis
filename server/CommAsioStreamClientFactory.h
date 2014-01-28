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

#ifndef COMMASIOSTREAMCLIENTFACTORY_H_
#define COMMASIOSTREAMCLIENTFACTORY_H_

#include "CommAsioClientKit.h"

class ServerRouting;

template <typename ClientT, typename ConnectionT>
class CommAsioStreamClientFactory : public CommAsioClientKit<ClientT>
{
    public:

        typedef ClientT client;

        CommAsioStreamClientFactory(ServerRouting& server);
        virtual ~CommAsioStreamClientFactory();

        virtual std::shared_ptr<ClientT> createClient(boost::asio::io_service& io_service);
        virtual void startClient(ClientT& client);

    protected:
        ServerRouting& mServer;

};

#endif /* COMMASIOSTREAMCLIENTFACTORY_H_ */
