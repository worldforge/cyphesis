// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "modules/WorldTime.h"

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;

    WorldTime worldTime;

    worldTime.update(0 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "winter");

    worldTime.update(1 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "winter");

    worldTime.update(2 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "spring");

    worldTime.update(3 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "spring");

    worldTime.update(4 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "spring");

    worldTime.update(5 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "summer");

    worldTime.update(6 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "summer");

    worldTime.update(7 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "summer");

    worldTime.update(8 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "autumn");

    worldTime.update(9 * DateTime::dpm() * DateTime::hpd() *
                         DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "autumn");

    worldTime.update(10 * DateTime::dpm() * DateTime::hpd() *
                          DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "autumn");

    worldTime.update(11 * DateTime::dpm() * DateTime::hpd() *
                          DateTime::mph() * DateTime::spm());

    assert(worldTime["season"] == "winter");

    return ret;
}
