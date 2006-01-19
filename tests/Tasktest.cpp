// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "rulesets/Fell.h"
#include "rulesets/Combat.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include <Atlas/Objects/RootOperation.h>

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;

    Operation op;
    OpVector res;

    {
        Task * task;

        if (0) {
            task->TickOperation(op, res);
        }
    }

    Entity ent1("1", 1), ent2("2", 2);
    Character chr("3", 3);

    {
        Fell fell(chr, ent1, ent2);

        assert(!fell.obsolete());

        fell.TickOperation(op, res);

        fell.irrelevant();

        assert(fell.obsolete());
    }

    Character chr1("4", 4), chr2("5", 5);

    {
        Combat * combat = new Combat(chr1, chr2);
        chr1.setTask(combat);
        chr2.setTask(combat);

        assert(!combat->obsolete());

        combat->TickOperation(op, res);

        combat->irrelevant();

        assert(combat->obsolete());
    }

    return ret;
}
