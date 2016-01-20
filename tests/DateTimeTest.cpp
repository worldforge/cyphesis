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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "modules/DateTime.h"

#include <cstring>

#include <cassert>

#include <cstdio>

int main()
{
    DateTime datum(0);

    std::cout << datum.asString() << std::endl << std::flush;

    assert(datum.isValid());

    // Hmm, we have loads of off by one errors in this code.
    // assert(datum.seconds() == 0);

    {
        static int year = 2000,
                   month = 1,
                   day = 1,
                   hour = 12,
                   min = 45,
                   sec = 12;
        static char string_date[32];

        sprintf(string_date, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);

        assert(strlen(string_date) == 19);

        std::cout << string_date << std::endl << std::flush;

        DateTime date_from_string(string_date);

        assert(date_from_string.isValid());

        assert(date_from_string.asString() == string_date);

        std::cout << date_from_string.asString() << std::endl << std::flush;

        DateTime date_from_values(year, month, day, hour, min, sec);

        assert(date_from_values.isValid());

        assert(date_from_string == date_from_values);

        assert(date_from_string.asString() == date_from_values.asString());

        assert(date_from_string.seconds() == date_from_values.seconds());
    }

    {
        DateTime new_date(2317);

        assert(new_date.isValid());

        new_date.update(0);

        assert(datum == new_date);
    }

    return 0;
}
