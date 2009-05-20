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

#include "rulesets/MemMap.h"
#include "rulesets/Script.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestMemMap : public MemMap {
  public:
    TestMemMap(Script * & s) : MemMap(s) { }

    MemEntity * test_addId(const std::string & id, long int_id) {
        return addId(id, int_id);
    }
};

int main()
{
    Script * s = &noScript;
    TestMemMap * mm = new TestMemMap(s);

    mm->test_addId("2", 2);

    OpVector res;
    mm->sendLooks(res);

    mm->del("2");

    delete mm;
    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}
