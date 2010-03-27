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

// $Id$

#include "common/AccountBase.h"

#include "common/log.h"

#include <cassert>

bool test_newid_fail = false;

int main()
{
    {
        AccountBase a;
    }

    {
        AccountBase * a = new AccountBase;

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        a->init();

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        a->putAccount(acc);

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = 1;
        a->putAccount(acc);

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        a->putAccount(acc);

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = 1;
        a->putAccount(acc);

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        a->putAccount(acc);

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        acc["type"] = "admin";
        a->putAccount(acc);

        delete a;
    }

    test_newid_fail = true;
    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["username"] = "fred";
        acc["password"] = "bob";
        a->putAccount(acc);

        delete a;
    }
    test_newid_fail = false;

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["type"] = 1;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["type"] = "admin";
        a->modAccount(acc, "1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["password"] = 1;
        a->modAccount(acc, "1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["password"] = "bill";
        a->modAccount(acc, "1");

        delete a;
    }
    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        acc["password"] = "bill";
        acc["type"] = "admin";
        a->modAccount(acc, "1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        a->delAccount("1");

        delete a;
    }

    {
        AccountBase * a = new AccountBase;

        Atlas::Message::MapType acc;
        a->getAccount("1", acc);

        delete a;
    }

    return 0;
}

// stubs

bool database_flag = true;

void log(LogLevel lvl, const std::string & msg)
{
}

Database * Database::m_instance = NULL;

Database * Database::instance()
{
    if (m_instance == NULL) {
        m_instance = new Database();
    }
    return m_instance;
}

Database::Database() : m_rule_db("rules"),
                       m_queryInProgress(false),
                       m_connection(NULL)
{
}

int Database::initConnection()
{
    return 0;
}

void Database::shutdownConnection()
{
}

long Database::newId(std::string & id)
{
    if (test_newid_fail) {
        return -1;
    }
    return 1;
}

const DatabaseResult Database::selectSimpleRowBy(const std::string & name,
                                                 const std::string & column,
                                                 const std::string & value)
{
    return DatabaseResult(0);
}

bool Database::updateSimpleRow(const std::string & name,
                               const std::string & key,
                               const std::string & value,
                               const std::string & columns)
{
    return true;
}

bool Database::createSimpleRow(const std::string & name,
                               const std::string & id,
                               const std::string & columns,
                               const std::string & values)
{
    return true;
}

const char * DatabaseResult::field(const char * column, int row) const
{
    return "";
}

void encrypt_password(const std::string & pwd, std::string & hash)
{
    hash = pwd;
}
