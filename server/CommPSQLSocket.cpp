// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "CommPSQLSocket.h"

#include "common/Database.h"
#include "common/log.h"
#include "common/debug.h"

#include <iostream>

#include <cassert>

static const bool debug_flag = false;

CommPSQLSocket::CommPSQLSocket(CommServer & svr, Database & db) :
                               CommIdleSocket(svr), m_db(db),
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

bool CommPSQLSocket::read()
{
    debug(std::cout << "CommPSQLSocket::read()" << std::endl << std::flush;);
    PGconn * con = m_db.getConnection();
    assert(con != 0);

    if (PQconsumeInput(con) == 0) {
        log(ERROR, "Error reading from database connection.");
        m_db.reportError();
    }

    PGresult * res;
    while (PQisBusy(con) == 0) {
        if ((res = PQgetResult(con)) != 0) {
            m_db.queryResult(PQresultStatus(res));
            PQclear(res);
        } else {
            m_db.queryComplete();
            return false;
        }
    };

    return false;
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
