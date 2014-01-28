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


#ifndef SERVER_COMM_ASIO_CLIENT_KIT_H
#define SERVER_COMM_ASIO_CLIENT_KIT_H

#include <string>
#include <memory>
#include <boost/asio/io_service.hpp>

/// \brief Abstract factory for creating objects from subclasses of CommClient
template<typename ClientT>
class CommAsioClientKit {
  public:
    virtual ~CommAsioClientKit() {};

    virtual std::shared_ptr<ClientT> createClient(boost::asio::io_service& io_service) = 0;
    virtual void startClient(ClientT& client) = 0;
};

#endif // SERVER_COMM_CLIENT_KIT_H
