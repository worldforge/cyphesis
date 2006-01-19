// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "CommPSQLSocket.h"

#include "common/Database.h"
#include "common/log.h"
#include "common/debug.h"

#include <iostream>

#include <cassert>

static const bool debug_flag = false;

/// \brief Constructor for PostgreSQL socket polling object.
///
/// @param svr Reference to the object that manages all socket communication.
/// @param db Reference to the low level database management object.
CommPSQLSocket::CommPSQLSocket(CommServer & svr, Database & db) :
                               Idle(svr), CommSocket(svr), m_db(db),
                               m_vacuumTime(0), m_reindexTime(0),
                               m_vacuumFull(false)
{
    // This assumes the database connection is already sorted, which I think
    // is okay
    PGconn * con = m_db.getConnection();
    assert(con != 0);
    
    if (PQsetnonblocking(con, 1) == -1) {
        log(ERROR, "Unable to put database connection in non-blocking mode.");
    }
}

CommPSQLSocket::~CommPSQLSocket()
{
}

int CommPSQLSocket::getFd() const
{
    debug(std::cout << "CommPSQLSocket::getFd()" << std::endl << std::flush;);
    PGconn * con = m_db.getConnection();
    assert(con != 0);
    return PQsocket(con);
}

bool CommPSQLSocket::eof()
{
    debug(std::cout << "CommPSQLSocket::eof()" << std::endl << std::flush;);
    return false;
}

bool CommPSQLSocket::isOpen() const
{
    debug(std::cout << "CommPSQLSocket::isOpen()" << std::endl << std::flush;);
    return true;
}

int CommPSQLSocket::read()
{
    debug(std::cout << "CommPSQLSocket::read()" << std::endl << std::flush;);
    PGconn * con = m_db.getConnection();
    assert(con != 0);

    if (PQconsumeInput(con) == 0) {
        log(ERROR, "Error reading from database connection.");
        m_db.reportError();
        
        log(ERROR, "Connection to RDBMS lost.");
        return 1;
    }

    PGresult * res;
    while (PQisBusy(con) == 0) {
        if ((res = PQgetResult(con)) != 0) {
            m_db.queryResult(PQresultStatus(res));
            PQclear(res);
        } else {
            m_db.queryComplete();
            return 0;
        }
    };

    return 0;
}

void CommPSQLSocket::dispatch()
{
    debug(std::cout << "CommPSQLSocket::dispatch()"
                    << std::endl << std::flush;);

    if (m_db.queryInProgress()) {
        return;
    }

    m_db.launchNewQuery();
}

void CommPSQLSocket::idle(time_t t)
{
    debug(std::cout << "CommPSQLSocket::idle()" << std::endl << std::flush;);

    if (t > m_vacuumTime) {
        if (m_vacuumFull) {
            m_db.runMaintainance(Database::MAINTAIN_VACUUM |
                                 Database::MAINTAIN_VACUUM_FULL);
        } else {
            m_db.runMaintainance(Database::MAINTAIN_VACUUM |
                                 Database::MAINTAIN_VACUUM_ANALYZE);
        }
        m_vacuumFull = !m_vacuumFull;
        m_vacuumTime = t + vacFreq;
    }
    if (t > m_reindexTime) {
        m_db.runMaintainance(Database::MAINTAIN_REINDEX);
        m_reindexTime = t + reindexFreq;
    }
}
