// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "common/id.h"

#include <iostream>

static int idGenerator = 0;

void newId(std::string & id)
{
    static char buf[32];
    int new_id = ++idGenerator;
    sprintf(buf, "%d", new_id);
    id = buf;
}
