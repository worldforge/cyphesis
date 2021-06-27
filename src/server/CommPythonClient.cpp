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


#include "CommPythonClient.h"

#include "pythonbase/PythonContext.h"

#include <iostream>

static const bool debug_flag = false;

CommPythonClient::CommPythonClient(const std::string& name,
                                   boost::asio::io_context& io_context) :
    mSocket(io_context),
    mBuffer{},
    m_pyContext(new PythonContext)
{
}

CommPythonClient::~CommPythonClient() = default;

boost::asio::local::stream_protocol::socket& CommPythonClient::getSocket()
{
    return mSocket;
}

void CommPythonClient::startAccept()
{
    do_read();
}

void CommPythonClient::do_read()
{
    auto self(this->shared_from_this());
    mSocket.async_read_some(boost::asio::buffer(mBuffer),
                            [this, self](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    read(length);
                                    //By calling do_read again we make sure that the instance
                                    //doesn't go out of scope ("shared_from this"). As soon as that
                                    //doesn't happen, and there's no do_write in progress, the instance
                                    //will be deleted since there's no more references to it.
                                    this->do_read();
                                }
                            });

}

void CommPythonClient::read(size_t bytes)
{

    for (size_t i = 0; i < bytes; ++i) {
        int next = mBuffer[i];
        if (next == '\n') {
            if (m_incoming.empty()) {
                std::cout << "[NOT]" << std::endl << std::flush;
            } else {
                // std::cout << m_incoming << std::endl << std::flush;
                std::cout << "[" << m_pyContext->runCommand(m_incoming)
                          << "]" << std::endl << std::flush;
                m_incoming.clear();
            }
        } else if (next == '\r') {
        } else {
            m_incoming.append(1, next);
        }
    }
}
