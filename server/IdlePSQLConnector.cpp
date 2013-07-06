// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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


#include "IdlePSQLConnector.h"

#include "CommServer.h"
#include "CommPSQLSocket.h"

#include "common/Database.h"
#include "common/log.h"

#include <iostream>

/// \brief Constructor for PSQL Connector.
///
/// @param svr Reference to the object that manages all socket communication.
IdlePSQLConnector::IdlePSQLConnector(CommServer & svr, Database & db) :
                   Idle(svr), m_db(db), m_lastConnect(svr.time())
{
}

IdlePSQLConnector::~IdlePSQLConnector()
{
}

void IdlePSQLConnector::idle(time_t time)
{
    if (time > m_lastConnect) {
        m_lastConnect = time;
        if (m_db.initConnection() == 0) {
            log(NOTICE, "Database connection re-established");
            CommPSQLSocket * dbsocket = new CommPSQLSocket(m_idleManager, m_db);
            m_idleManager.addSocket(dbsocket);
            m_idleManager.addIdle(dbsocket);
            delete this;
        }
    }
}
