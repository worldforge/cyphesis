// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

// $Id: newid.cpp,v 1.5 2006-10-26 00:48:05 alriddoch Exp $

#include "common/id.h"
#include "common/const.h"
#include "common/Database.h"

#include <assert.h>

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
        assert(!id.empty());
        return new_id;
    }
}
