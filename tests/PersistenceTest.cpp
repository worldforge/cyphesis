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

#include "rulesets/LocatedEntity.h"
#include "server/Persistence.h"
#include "DatabaseNull.h"

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

    {
        DatabaseNull database;
        Persistence p(database);
        int res = p.init();
        assert(res == 0);
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


#include "stubs/common/stubDatabase.h"

const char * const CYPHESIS = "cyphesis";

std::string instance("deeds");

#include "stubs/common/stubRouter.h"
#include "stubs/server/stubAccount.h"

ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

const char * ServerAccount::getType() const
{
    return "server";
}

int ServerAccount::characterError(const Operation & op,
                                  const Root & ent,
                                  OpVector & res) const
{
    return -1;
}

void ServerAccount::createObject(const Root & arg,
                                 const Operation & op,
                                 OpVector & res)
{
}

#include "stubs/server/stubAdmin.h"
#include "stubs/server/stubPlayer.h"


Shaker::Shaker() {}

std::string Shaker::generateSalt(size_t)
{
    return "";
}

void log(LogLevel lvl, const std::string & msg)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

