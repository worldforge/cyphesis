// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_RANDOM_H
#define COMMON_RANDOM_H

extern "C" {
    #include <stdlib.h>
}

static inline int randint(int min, int max)
{
    return rand() % (max - min) + min;
}

static double uniform(double min, double max)
{
    return ((double)rand() / RAND_MAX) * (max - min) + min;
}

#endif // COMMON_RANDOM_H
