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

#include "common/Database.h"

#include "common/const.h"
#include "common/compose.hpp"
#include "common/log.h"

#include <cstdlib>

#include <cassert>

int main()
{
    {
        assert(Database::instance() != 0);

        Database::cleanup();
    }

    {
        assert(Database::instance()->getConnection() == 0);

        Database::cleanup();
    }

    {
        assert(Database::instance()->rule() == "rules");

        Database::cleanup();
    }

    {
        assert(Database::instance()->queryInProgress() == false);

        Database::cleanup();
    }

    {
        // Database::instance()->decodeObject();

        Database::cleanup();
    }

    {
        // Database::instance()->encodeObject();

        Database::cleanup();
    }


    return 0;
}

const char * CYPHESIS = "cyphesis";
std::string instance("test_instance");

namespace consts {
  const long rootWorldIntId = 0L;
}

void log(LogLevel lvl, const std::string & msg)
{
}

void log_formatted(LogLevel lvl, const std::string & msg)
{
}

long forceIntegerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        log(CRITICAL, String::compose("Unable to convert ID \"%1\" to an integer", id));
        abort();
    }

    return intId;
}

template <typename T>
int readConfigItem(const std::string & section, const std::string & key, T & storage)
{
    return -1;
}

template<>
int readConfigItem<std::string>(const std::string & section, const std::string & key, std::string & storage)
{
    return -1;
}
