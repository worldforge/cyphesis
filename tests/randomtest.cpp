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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/random.h"

#include <algorithm>
#include <iostream>

#include <cassert>

int main()
{
    int int1 = randint(-2000000, 2000000);
    int int2 = randint(-2000000, 2000000);
    int previ = randint(std::min(int1, int2), std::max(int1, int2));

    assert(int1 != int2);
    assert(previ != int1);
    assert(previ != int2);

    float float1 = uniform(-2000000.f, 2000000.f);
    float float2 = uniform(-2000000.f, 2000000.f);
    float prevf = uniform(std::min(float1, float2), std::max(float1, float2));

    assert(float1 != float2);
    assert(prevf != float1);
    assert(prevf != float2);

    for (int i = 0; i < 1000; ++i) {
        int1 = randint(-2000000, 2000000);
        int2 = randint(-2000000, 2000000);

        assert(int1 != int2);

        float1 = uniform(-2000000.f, 2000000.f);
        float2 = uniform(-2000000.f, 2000000.f);

        assert(float1 != float2);

        int num = randint(std::min(int1, int2), std::max(int1, int2));
        std::cout << num << ":" << previ << " " << int1 << ":" << int2
                  << std::endl << std::flush;
        assert(num != int1);
        assert(num != int2);
        assert(num != previ);
        assert(num > std::min(int1, int2));
        assert(num < std::max(int1, int2));
        previ = num;

        float fl = uniform(std::min(float1, float2), std::max(float1, float2));
        assert(fl != prevf);
        assert(fl != float1);
        assert(fl != float2);
        assert(fl > std::min(float1, float2));
        assert(fl < std::max(float1, float2));
        prevf = fl;
    }
}
