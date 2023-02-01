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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/Storage.h"

#include "common/system.h"
#include "../DatabaseNull.h"

#include <cassert>

bool test_newid_fail = false;

int main()
{
    {
        DatabaseNull database;
        Storage a(database);
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        a->putAccount(acc);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = 1;
        a->putAccount(acc);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        a->putAccount(acc);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = 1;
        a->putAccount(acc);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        a->putAccount(acc);

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        acc["type"] = "admin";
        a->putAccount(acc);

        delete a;
    }

    test_newid_fail = true;
    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        a->putAccount(acc);

        delete a;
    }
    test_newid_fail = false;

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["type"] = 1;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["type"] = "admin";
        a->modAccount(acc, "1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["password"] = 1;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["password"] = "bill";
        a->modAccount(acc, "1");

        delete a;
    }
    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        acc["password"] = "bill";
        acc["type"] = "admin";
        a->modAccount(acc, "1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        a->delAccount("1");

        delete a;
    }

    {
        DatabaseNull database;
        Storage * a = new Storage(database);

        Atlas::Message::MapType acc;
        a->getAccount("1", acc);

        delete a;
    }

    return 0;
}

// stubs

#include "../stubs/common/stublog.h"
#include "../stubs/common/stubShaker.h"
#include "../stubs/common/stubglobals.h"

#define STUB_Database_newId
long Database::newId()
{
    if (test_newid_fail) {
        return -1;
    }
    return 1;
}


#define STUB_Database_selectSimpleRowBy
DatabaseResult Database::selectSimpleRowBy(const std::string & name, const std::string & column, const std::string & value)
{
    return DatabaseResult(std::unique_ptr<DatabaseNullResultWorker>(new DatabaseNullResultWorker()));
}

#include "../stubs/common/stubDatabase.h"


void encrypt_password(const std::string & pwd, std::string & hash)
{
    hash = pwd;
}
