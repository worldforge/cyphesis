// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/id.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <cassert>

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        log(CRITICAL, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
        abort();
    }

    return intId;
}

int integerIdCheck(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        return -1;
    }
    return 0;
}
