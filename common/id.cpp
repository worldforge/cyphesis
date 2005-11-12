// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/id.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <iostream>

#include <cassert>

static long idGenerator = 0;

void newId(std::string & id)
{
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        log(CRITICAL, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
        abort();
    }

    return intId;
}
