// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_COMM_PSQL_SOCKET_H
#define SERVER_COMM_PSQL_SOCKET_H

#include "CommIdleSocket.h"

class Database;

class CommPSQLSocket : public CommIdleSocket {
  protected:
    Database & m_db;

    time_t m_vacuumTime;
    time_t m_reindexTime;;
    bool m_vacuumFull;
  public:
    static const int vacFreq = 25 * 60;
    static const int reindexFreq = 30 * 60;

    CommPSQLSocket(CommServer & svr, Database & db);
    virtual ~CommPSQLSocket();

    virtual int getFd() const;
    virtual bool eof();
    virtual bool isOpen() const;

    virtual bool read();
    virtual void dispatch();

    virtual void idle(time_t t);
};

#endif // SERVER_COMM_PSQL_SOCKET_H
