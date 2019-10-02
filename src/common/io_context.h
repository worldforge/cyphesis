/*
 Copyright (C) 2019 Erik Ogenvik

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

#ifndef CYPHESIS_IO_CONTEXT_H
#define CYPHESIS_IO_CONTEXT_H

/**
 * Boost version 1.66+ changed io_service to io_context. This is a shim.
 */

#include <boost/version.hpp>
#if BOOST_VERSION < 106600
#include <boost/asio/io_service.hpp>
namespace boost {
namespace asio {
typedef io_service io_context;
}
}
#else
#include <boost/asio/io_context.hpp>
#endif

#endif //CYPHESIS_IO_CONTEXT_H
