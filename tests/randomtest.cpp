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

// $Id: randomtest.cpp,v 1.3 2006-10-26 00:48:16 alriddoch Exp $

#include "common/random.h"

#include <algorithm>
#include <iostream>

#include <cassert>

int main()
{
    int int1 = randint(-2000000, 2000000);
    int int2 = randint(-2000000, 2000000);
    int previ = randint(std::min(int1, int2), std::max(int1, int2));

    float float1 = uniform(-2000000.f, 2000000.f);
    float float2 = uniform(-2000000.f, 2000000.f);
    float prevf = uniform(std::min(float1, float2), std::max(float1, float2));

    for (int i = 0; i < 1000; ++i) {
        int int1 = randint(-2000000, 2000000);
        int int2 = randint(-2000000, 2000000);

        float float1 = uniform(-2000000.f, 2000000.f);
        float float2 = uniform(-2000000.f, 2000000.f);

        int num = randint(std::min(int1, int2), std::max(int1, int2));
        std::cout << num << ":" << previ << " " << int1 << ":" << int2 << std::endl << std::flush;
        assert(num != previ);
        previ = num;

        float fl = uniform(std::min(float1, float2), std::max(float1, float2));
        assert(fl != prevf);
        prevf = fl;
    }
}
