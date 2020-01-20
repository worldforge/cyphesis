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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/id.h"

#include "common/log.h"

#include <iostream>

#include <cstdlib>

#include <setjmp.h>
#include <signal.h>

#include <cassert>

jmp_buf env;

static void handle_abort(int)
{
    longjmp(env, 1);
}

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

        assert(integerIdCheck(text) == -1);
    }

    {
        std::string text("text");

        assert(integerId(text) == -1);
    }

    if (setjmp(env) == 0) {
        signal(SIGABRT, handle_abort);

        // This should abort due to hitting an abort call
        {
            std::string text("text");

            forceIntegerId(text);
        }

        // The abort causes control to go back to setjmp
        return 1;
    }

    std::cout << "It worked" << std::endl << std::flush;


    return 0;
}
#include "../stubs/common/stublog.h"
