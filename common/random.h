// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2003 Alistair Riddoch

#ifndef COMMON_RANDOM_H
#define COMMON_RANDOM_H

extern "C" {
    #include <stdlib.h>
}

static inline int randint(int min, int max)
{
    if (max == min) {
        return min;
    } else {
        return rand() % (max - min) + min;
    }
}

static inline float uniform(float min, float max)
{
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}

#endif // COMMON_RANDOM_H
