#include <memory>

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "rules/LocatedEntity.h"
#include "server/Persistence.h"
#include "../DatabaseNull.h"

#include <Atlas/Message/Element.h>

#include <cassert>
#include <common/Database.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;

int main()
{
    {
        DatabaseNull database;
        Persistence p(database);
    }
    return 0;
}

// stubs

#include "server/Admin.h"
#include "server/Player.h"
#include "server/ServerAccount.h"

#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/id.h"
#include "common/Shaker.h"


#define STUB_Database_selectSimpleRowBy
DatabaseResult Database::selectSimpleRowBy(const std::string& name,
                                           const std::string& column,
                                           const std::string& value)
{
    return DatabaseResult(std::make_unique<DatabaseNullResultWorker>());
}


#include "../stubs/common/stubDatabase.h"

const char * const CYPHESIS = "cyphesis";

std::string instance("deeds");

#include "../stubs/common/stubRouter.h"
#include "../stubs/server/stubAccount.h"

ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             RouterId id) :
       Account(conn, username, passwd, id)
{
}

const char * ServerAccount::getType() const
{
    return "server";
}


#include "../stubs/server/stubAdmin.h"
#include "../stubs/server/stubPlayer.h"
#include "../stubs/rules/simulation/stubExternalMind.h"


Shaker::Shaker() {}

std::string Shaker::generateSalt(size_t)
{
    return "";
}
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubid.h"
