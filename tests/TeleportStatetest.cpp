// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "server/TeleportState.h"

#include <cassert>

int main()
{
    {
        TeleportState * ts = new TeleportState(std::chrono::steady_clock::now());

        delete ts;
    }

    {
        auto create_time = std::chrono::steady_clock::now() + std::chrono::seconds(23);

        TeleportState * ts = new TeleportState(create_time);

        assert(ts->getCreateTime() == create_time);

        delete ts;
    }

    {
        TeleportState * ts = new TeleportState(std::chrono::steady_clock::now());

        assert(!ts->isCreated());
        ts->setCreated();
        assert(ts->isCreated());

        delete ts;
    }

    {
        TeleportState * ts = new TeleportState(std::chrono::steady_clock::now());

        assert(!ts->isRequested());
        ts->setRequested();
        assert(ts->isRequested());

        delete ts;
    }

    {
        TeleportState * ts = new TeleportState(std::chrono::steady_clock::now());
        std::string test_key_val("whsiehibsfw");

        assert(ts->getPossessKey().empty());
        assert(!ts->isMind());
        ts->setKey(test_key_val);
        assert(ts->isMind());
        assert(ts->getPossessKey() == test_key_val);

        delete ts;
    }

    return 0;
}
