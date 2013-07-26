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

#include "server/PendingPossession.h"

#include <cassert>

int main()
{
    {
        PendingPossession * pt = new PendingPossession("", "");

        delete pt;
    }

    {
        std::string test_id("de214cec-f8c4-11df-baf7-00269e5444b3");
        PendingPossession * pt = new PendingPossession(test_id, "");

        const std::string & entity_id = pt->getEntityID();
        assert(entity_id == test_id);

        delete pt;
    }

    {
        std::string test_key("3bf18e18-f8c5-11df-b0bf-00269e5444b3");
        PendingPossession * pt = new PendingPossession("", test_key);

        const std::string & key = pt->getPossessKey();
        assert(key == test_key);

        delete pt;
    }

    {
        std::string test_id("de214cec-f8c4-11df-baf7-00269e5444b3");
        std::string test_key("3bf18e18-f8c5-11df-b0bf-00269e5444b3");
        PendingPossession * pt = new PendingPossession(test_id, test_key);

        bool ret = pt->validate(test_id, test_key);
        assert(ret);

        delete pt;
    }

    {
        std::string test_id("de214cec-f8c4-11df-baf7-00269e5444b3");
        std::string test_key("3bf18e18-f8c5-11df-b0bf-00269e5444b3");
        PendingPossession * pt = new PendingPossession(test_id, test_key);

        bool ret = pt->validate(test_id, "c0e3b16e-f8c5-11df-9070-00269e5444b3");
        assert(!ret);

        delete pt;
    }

    {
        std::string test_id("de214cec-f8c4-11df-baf7-00269e5444b3");
        std::string test_key("3bf18e18-f8c5-11df-b0bf-00269e5444b3");
        PendingPossession * pt = new PendingPossession(test_id, test_key);

        bool ret = pt->validate("c7e27496-f8c5-11df-9103-00269e5444b3", test_key);
        assert(!ret);

        delete pt;
    }

    {
        std::string test_id("de214cec-f8c4-11df-baf7-00269e5444b3");
        std::string test_key("3bf18e18-f8c5-11df-b0bf-00269e5444b3");
        PendingPossession * pt = new PendingPossession(test_id, test_key);

        bool ret = pt->validate("d6dd2626-f8c5-11df-853f-00269e5444b3", "e13c51be-f8c5-11df-a97f-00269e5444b3");
        assert(!ret);

        delete pt;
    }

    {
        PendingPossession * pt = new PendingPossession("", "");

        assert(!pt->isValidated());

        delete pt;
    }

    {
        PendingPossession * pt = new PendingPossession("", "");

        assert(!pt->isValidated());
        pt->setValidated();
        assert(pt->isValidated());

        delete pt;
    }

    return 0;
}
