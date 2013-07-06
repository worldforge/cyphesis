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


#include "ArithmeticBuilder.h"

#include "rulesets/PythonArithmeticFactory.h"

ArithmeticBuilder * ArithmeticBuilder::m_instance = 0;

ArithmeticBuilder::ArithmeticBuilder()
{
}

ArithmeticBuilder * ArithmeticBuilder::instance()
{
    if (m_instance == 0) {
        m_instance = new ArithmeticBuilder;
    }
    return m_instance;
}

void ArithmeticBuilder::del()
{
    if (m_instance != 0) {
        delete m_instance;
        m_instance = 0;
    }
}

ArithmeticScript * ArithmeticBuilder::newArithmetic(const std::string & name,
                                                    LocatedEntity * owner)
{
    PythonArithmeticFactory paf("world.statistics.Statistics", "Statistics");
    paf.setup();
    return paf.newScript(owner);
}
