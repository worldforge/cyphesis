// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_PSQL_SOCKET_H
#define SERVER_COMM_PSQL_SOCKET_H

#include "CommIdleSocket.h"

class Database;

/// \brief Handle polling the socket used to comminicate with the PostgreSQL
/// RDBMS.
class CommPSQLSocket : public CommIdleSocket {
  protected:
    /// Reference to the low level database management object.
    Database & m_db;

    /// Time when the database vacuum job should be run.
    time_t m_vacuumTime;
    /// Time when the database reindex job should be run.
    time_t m_reindexTime;;
    /// Flag indicating whether the next vacuum job should be vacuum full.
    bool m_vacuumFull;
  public:
    /// Interval between database vacuum jobs.
    static const int vacFreq = 25 * 60;
    /// Interval between database reindex jobs.
    static const int reindexFreq = 30 * 60;

    CommPSQLSocket(CommServer & svr, Database & db);
    virtual ~CommPSQLSocket();

    int getFd() const;
    bool isOpen() const;
    bool eof();
    int read();
    void dispatch();

    void idle(time_t t);
};

#endif // SERVER_COMM_PSQL_SOCKET_H
