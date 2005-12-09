// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
