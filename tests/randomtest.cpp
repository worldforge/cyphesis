// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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
