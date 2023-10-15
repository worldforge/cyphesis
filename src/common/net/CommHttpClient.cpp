// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#include "CommHttpClient.h"

static const bool debug_flag = false;

CommHttpClient::CommHttpClient(const std::string& name,
                               boost::asio::io_context& io_context,
                               HttpRequestProcessor& requestProcessor) :
    mSocket(io_context),
    mStream(&mBuffer),
    m_requestProcessor(requestProcessor)
{
}

CommHttpClient::~CommHttpClient() = default;

void CommHttpClient::serveRequest()
{
    do_read();
}

void CommHttpClient::do_read()
{
    auto self(this->shared_from_this());
    mSocket.async_read_some(mBuffer.prepare(1024),
                            [this, self](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    mBuffer.commit(length);
                                    bool complete = read();
                                    if (complete) {
                                        write();
                                    }
                                    //By calling do_read again we make sure that the instance
                                    //doesn't go out of scope ("shared_from this"). As soon as that
                                    //doesn't happen, and there's no do_write in progress, the instance
                                    //will be deleted since there's no more references to it.
                                    this->do_read();
                                }
                            });

}

void CommHttpClient::write()
{
    m_requestProcessor.processQuery(mStream, m_headers);
    mStream << std::flush;
    auto self(this->shared_from_this());
    boost::asio::async_write(mSocket, mBuffer.data(),
                             [this, self](boost::system::error_code ec, std::size_t length) {
                                 if (!ec) {
                                 }
                                 mSocket.close();
                             });
}

bool CommHttpClient::read()
{

    std::streamsize count;

    while ((count = mStream.rdbuf()->in_avail()) > 0) {

        for (int i = 0; i < count; ++i) {

            int next = mStream.rdbuf()->sbumpc();
            if (next == '\n') {
                if (m_incoming.empty()) {
                    return true;
                } else {
                    m_headers.push_back(m_incoming);
                    m_incoming.clear();
                }
            } else if (next == '\r') {
            } else {
                m_incoming.append(1, next);
            }
        }
    }

    // Read from the sockets.

    return false;
}

boost::asio::ip::tcp::socket& CommHttpClient::getSocket()
{
    return mSocket;
}
