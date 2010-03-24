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

#include "common/id.h"

#include "common/Database.h"
#include "common/globals.h"
#include "common/log.h"

#include <cassert>

int main()
{
    database_flag = false;

    std::string id;

    long int_id = newId(id);

    assert(!id.empty());
    assert(int_id > 0);

    database_flag = true;

    int_id = newId(id);

    assert(!id.empty());
    assert(int_id > 0);

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

long Database::newId(std::string & id)
{
    return 1;
}
