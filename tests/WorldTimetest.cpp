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

// $Id: WorldTimetest.cpp,v 1.3 2006-10-26 00:48:16 alriddoch Exp $

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
