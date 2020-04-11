// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003-2004 Alistair Riddoch
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


#ifndef SERVER_COMM_PSQL_SOCKET_H
#define SERVER_COMM_PSQL_SOCKET_H

#include "CommSocket.h"

#include "asio.h"
#include <boost/asio/steady_timer.hpp>
#include <boost/noncopyable.hpp>

class DatabasePostgres;

/// \brief Handle polling the socket used to communicate with the PostgreSQL
/// RDBMS.
/// \ingroup ServerSockets
class CommPSQLSocket : private boost::noncopyable {
  protected:

    boost::asio::io_context& m_io_context;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
    boost::asio::steady_timer m_vacuumTimer;
    boost::asio::steady_timer m_reindexTimer;
    boost::asio::steady_timer m_reconnectTimer;


    /// Reference to the low level database management object.
    DatabasePostgres & m_db;

    /// Flag indicating whether the next vacuum job should be vacuum full.
    bool m_vacuumFull;

    void do_read();
    int read();
    void dispatch();

    void vacuum();
    void reindex();

    void tryReConnect();
  public:
    /// Interval between database vacuum jobs.
    static const int vacFreq;
    /// Interval between database reindex jobs.
    static const int reindexFreq;

    CommPSQLSocket(boost::asio::io_context& io_context, DatabasePostgres & db);
    virtual ~CommPSQLSocket();

    void cancel();
};

#endif // SERVER_COMM_PSQL_SOCKET_H
