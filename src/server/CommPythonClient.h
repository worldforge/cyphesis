// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef SERVER_COMM_PYTHON_CLIENT_H
#define SERVER_COMM_PYTHON_CLIENT_H

#include "common/asio.h"
#include <boost/array.hpp>

#include <memory>
#include <string>
#include <Atlas/Objects/Factories.h>

class PythonContext;

/// \brief Handle an internet socket connected to a remote python commandline.
/// \ingroup ServerSockets
class CommPythonClient : public std::enable_shared_from_this<CommPythonClient> {
  protected:

    boost::asio::local::stream_protocol::socket mSocket;

    /**
     * \brief Receiving buffer.
     *
     * Note that std::array isn't used to keep compatibility with older versions of boost.
     */
    boost::array<char, 256> mBuffer;

    std::unique_ptr<PythonContext> m_pyContext;
    std::string m_incoming;

    void read(size_t bytes);
    void do_read();

  public:
    CommPythonClient(const std::string & name,
            boost::asio::io_context& io_context);
    virtual ~CommPythonClient();

    void startAccept();
    boost::asio::local::stream_protocol::socket& getSocket();
};

#endif // SERVER_COMM_PYTHON_CLIENT_H
