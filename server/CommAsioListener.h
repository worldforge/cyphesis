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

#include <boost/asio.hpp>

class ServerRouting;
template<typename ProtocolT, typename ClientT, typename ConnectionT>
class CommAsioListener
{
    public:
        CommAsioListener(ServerRouting& server,
                boost::asio::io_service& ioService,
                const typename ProtocolT::endpoint& endpoint);
        virtual ~CommAsioListener();
    protected:
        ServerRouting& mServer;

        typename ProtocolT::acceptor mAcceptor;

        void startAccept();
};

#endif /* COMMASIOLISTENER_H_ */
