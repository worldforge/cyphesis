// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "CommPSQLSocket.h"

#include "common/Database.h"
#include "common/log.h"

CommPSQLSocket::CommPSQLSocket(CommServer & svr, Database & db) :
                               CommIdleSocket(svr), m_db(db)
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
    PGconn * con = m_db.getConnection();
    assert(con != 0);
    return PQsocket(con);
}

bool CommPSQLSocket::eof()
{
    return false;
}

bool CommPSQLSocket::isOpen() const
{
    return true;
}

bool CommPSQLSocket::read()
{
    PGconn * con = m_db.getConnection();
    assert(con != 0);

    if (PQconsumeInput(con) == 0) {
        log(ERROR, "Error reading from database connection.");
        m_db.reportError();
    }

    if (PQisBusy(con) == 1) {
        return false;
    }

    // Lets get some results, using PQgetResult(con);

    return false;
}

void CommPSQLSocket::dispatch()
{
    PGconn * con = m_db.getConnection();
    assert(con != 0);

    // Here we are permited to write
    // Perhaps PQflush(con); ?
}

void CommPSQLSocket::idle(time_t t)
{
}
