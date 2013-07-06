// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2003 Alistair Riddoch
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


#ifndef COMMON_RANDOM_H
#define COMMON_RANDOM_H

#include <cstdlib>

static inline int randint(int min, int max)
{
    if (max == min) {
        return min;
    } else {
        return ::rand() % (max - min) + min;
    }
}

static inline float uniform(float min, float max)
{
    return ((float)::rand() / RAND_MAX) * (max - min) + min;
}

#endif // COMMON_RANDOM_H
