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

#include "rulesets/ArithmeticFactory.h"
#include "rulesets/StatisticsProperty.h"

#include "common/globals.h"

int main(int argc, char ** argv)
{
    StatisticsProperty * ap = new StatisticsProperty;

    PropertyCoverage pc(ap);

    // Coverage is complete, but it wouldn't hurt to add some bad data here.

    pc.basicCoverage();

    // The is no code in operations.cpp to execute, but we need coverage.
    return 0;
}

// stubs

ArithmeticKit::~ArithmeticKit()
{
}

PythonArithmeticFactory::PythonArithmeticFactory(const std::string & package,
                                                 const std::string & name) :
                                                 m_module(0), m_class(0),
                                                 m_package(package),
                                                 m_type(name)
{
}

PythonArithmeticFactory::~PythonArithmeticFactory()
{
}

ArithmeticScript * PythonArithmeticFactory::newScript(Entity * owner)
{
    return 0;
}
