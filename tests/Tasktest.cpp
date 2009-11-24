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

// $Id$

#include "Fell.h"

#include "rulesets/Entity.h"
#include "rulesets/Character.h"

#include <Atlas/Objects/Generic.h>
#include <Atlas/Objects/RootEntity.h>

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;

    Operation op;

    {
        Task * task;
        OpVector res;

        if (0) {
            task->TickOperation(op, res);
        }
    }

    Entity ent1("1", 1), ent2("2", 2);
    Character chr("3", 3);

    {
        Fell fell(chr, ent1, ent2);

        fell.nextTick(1.5);

        Atlas::Message::Element val;
        fell.getAttr("foo", val);
        assert(val.isNone());
        fell.setAttr("foo", 1);
        fell.getAttr("foo", val);
        assert(val.isInt());

        Atlas::Objects::Entity::RootEntity ent;
        fell.addToEntity(ent);

        fell.progress() = 0.5f;
        fell.addToEntity(ent);

        fell.rate() = 0.5f;
        fell.addToEntity(ent);

        assert(!fell.obsolete());

        OpVector res;

        assert(res.empty());

        Atlas::Objects::Operation::Generic c;

        fell.initTask(c, res);

        assert(!res.empty());

        fell.TickOperation(op, res);

        fell.irrelevant();

        assert(fell.obsolete());
    }

    return ret;
}
