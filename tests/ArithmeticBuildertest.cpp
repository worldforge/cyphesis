// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/ArithmeticBuilder.h"

#include "rulesets/PythonArithmeticFactory.h"

#include <cassert>

int main()
{
    {
        ArithmeticBuilder * ab = ArithmeticBuilder::instance();
        assert(ab != 0);
    }

    {
        ArithmeticBuilder * ab = ArithmeticBuilder::instance();
        assert(ab != 0);
        ArithmeticBuilder::del();
    }

    {
        ArithmeticBuilder * ab = ArithmeticBuilder::instance();
        assert(ab != 0);

        ArithmeticScript * as = ab->newArithmetic("foo", 0);
        // The factory this uses is stubbed out to return 0.
        assert(as == 0);
    }

}

// stubs

PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 PythonClass(package,
                                                             name,
                                                             0)
{
}

PythonArithmeticFactory::~PythonArithmeticFactory()
{
}

int PythonArithmeticFactory::setup()
{
    return 0;
}

ArithmeticScript * PythonArithmeticFactory::newScript(LocatedEntity * owner)
{
    return 0;
}

ArithmeticKit::~ArithmeticKit()
{
}

PythonClass::PythonClass(const std::string & package,
                         const std::string & type,
                         struct _typeobject * base)
{
}

PythonClass::~PythonClass()
{
}
