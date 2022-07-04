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


#include <Atlas/Objects/Operation.h>
#include "server/SystemAccount.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "rules/simulation/BaseWorld.h"
#include "rules/LocatedEntity.h"

SystemAccount::SystemAccount(Connection* conn,
                             const std::string& username,
                             const std::string& passwd,
                             RouterId id) :
        Admin(conn, username, passwd, std::move(id))
{
}

SystemAccount::~SystemAccount() = default;

const char* SystemAccount::getType() const
{
    return "sys";
}

bool SystemAccount::isPersisted() const
{
    return false;
}

void SystemAccount::processExternalOperation(const Operation& op, OpVector& res)
{
    //Allow system accounts to send operations directly to other entities.
    if (!op->isDefaultTo() && op->getTo() != getId()) {
            auto entity = m_connection->m_server.getWorld().getEntity(op->getTo());
            if (entity) {
                entity->operation(op, res);
            }
            return;
    } else {
        Account::processExternalOperation(op, res);
    }
}
