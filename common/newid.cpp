// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/id.h"
#include "common/const.h"
#include "common/Database.h"

static long idGenerator = 0;

long newId(std::string & id)
{
    if (consts::enable_database) {
        return Database::instance()->newId(id);
    } else {
        static char buf[32];
        long new_id = ++idGenerator;
        sprintf(buf, "%ld", new_id);
        id = buf;
        return new_id;
    }
}
