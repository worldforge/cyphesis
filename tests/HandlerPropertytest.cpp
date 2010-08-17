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

#include "PropertyCoverage.h"

#include "rulesets/HandlerProperty.h"
#include "rulesets/Entity.h"

#include <Atlas/Objects/Operation.h>

static HandlerResult test_handler(Entity *, const Operation &, OpVector &)
{
    return OPERATION_HANDLED;
}

int main()
{
    PropertyBase * ap = new HandlerProperty<int>(Atlas::Objects::Operation::GET_NO, &test_handler);

    {
        PropertyCoverage pc(ap);

        pc.basicCoverage();
    }

    ap = new HandlerProperty<double>(Atlas::Objects::Operation::GET_NO, &test_handler);

    {
        PropertyCoverage pc(ap);

        pc.basicCoverage();
    }

    ap = new HandlerProperty<std::string>(Atlas::Objects::Operation::GET_NO, &test_handler);

    {
        PropertyCoverage pc(ap);

        pc.basicCoverage();
    }

    return 0;
}

// stubs

void Entity::installHandler(int class_no, Handler handler)
{
}
