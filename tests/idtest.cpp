// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id$

#include "common/id.h"

#include <cassert>

int main()
{
    {
        std::string one("1");

        assert(integerId(one) == 1);
    }

    {
        std::string two("2");

        assert(integerId(two) == 2);
    }

    {
        std::string minus_two("-2");

        assert(integerId(minus_two) == -2);
    }

    {
        std::string text("text");

        assert(integerId(text) == -1L);
    }

    {
        std::string one("1");

        assert(forceIntegerId(one) == 1);
    }

    {
        std::string two("2");

        assert(forceIntegerId(two) == 2);
    }

    {
        std::string minus_two("-2");

        assert(forceIntegerId(minus_two) == -2);
    }

    {
        std::string one("1");

        assert(integerIdCheck(one) == 0);
    }

    {
        std::string two("2");

        assert(integerIdCheck(two) == 0);
    }

    {
        std::string minus_two("-2");

        assert(integerIdCheck(minus_two) == 0);
    }

    {
        std::string text("text");

        assert(integerId(text) == -1);
    }

    return 0;
}
