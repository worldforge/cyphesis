// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "rulesets/TaskScript.h"
#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/Script.h"

#include <Atlas/Objects/Generic.h>

#include <iostream>

#include <cassert>

class TestScript : public Script {
  public:
    TestScript() { }

    virtual bool operation(const std::string & opname,
                           const Atlas::Objects::Operation::RootOperation & op,
                           OpVector & res) { return true; }
};

int main()
{
    int ret = 0;

    Operation op;

    Character chr("3", 3);

    {
        TaskScript ts(chr);

        OpVector res;
        Atlas::Objects::Operation::Generic c;

        // It has no script, so init will fail, and it will be irrelevant
        ts.initTask(c, res);

        assert(ts.obsolete());
    }

    {
        TaskScript ts(chr);

        Script * script1 = new Script;
        Script * script2 = new Script;

        ts.setScript(script1);
        ts.setScript(script2);

        assert(!ts.obsolete());

        OpVector res;
        Atlas::Objects::Operation::Generic c;

        ts.initTask(c, res);

        // It has useless script, so init will fail, and it will be irrelevant
        ts.TickOperation(op, res);

        assert(ts.obsolete());
    }

    {
        TaskScript ts(chr);

        Script * script1 = new TestScript;

        ts.setScript(script1);

        assert(!ts.obsolete());

        OpVector res;
        Atlas::Objects::Operation::Generic c;

        assert(res.empty());

        ts.initTask(c, res);

        // It has useless script, so init will fail, and it will be irrelevant
        ts.TickOperation(op, res);

        assert(!ts.obsolete());
        assert(!res.empty());
    }

    return ret;
}
