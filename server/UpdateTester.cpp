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

// $Id$

#include "UpdateTester.h"

#include "CommServer.h"
#include "Connection.h"
#include "ServerRouting.h"
#include "Lobby.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/OperationRouter.h"

#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Objects::Operation::Info;
using Atlas::Objects::Root;

UpdateTester::UpdateTester(CommServer & svr) : Idle(svr)
{
}

UpdateTester::~UpdateTester()
{
}

void UpdateTester::idle(time_t t)
{
    Atlas::Objects::Root o = Inheritance::instance().getClass("oak");
    if (!o.isValid()) {
        log(WARNING, "No type update to send");
        return;
    }

    Info info;
    info->setArgs1(o);

    ServerRouting & svr = m_idleManager.m_server;

    AccountDict::const_iterator I = svr.m_lobby.getAccounts().begin();
    AccountDict::const_iterator Iend = svr.m_lobby.getAccounts().end();
    for (; I != Iend; ++I) {
        Connection * c = I->second->m_connection;
        if (c != 0) {
            c->send(info);
        }
    }

}
