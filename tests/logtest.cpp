// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "common/log.h"
#include "common/globals.h"

#include <cassert>

int main()
{
    assert(daemon_flag == false);

    log(INFO, "Info log message.");
    log(SCRIPT, "Script log message.");
    log(NOTICE, "Notice log message.");
    log(WARNING, "Warning log message.");
    log(SCRIPT_ERROR, "Script Error log message.");
    log(CRITICAL, "Critical log message.");

    return 0;
}
