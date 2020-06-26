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

#include "rules/Script.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
using Atlas::Message::MapType;

int main()
{
    {
        Script test_script;
        OpVector res;
        Atlas::Objects::Operation::Get op;

        bool ret = test_script.operation("op", op, res);
        assert(!ret);

        test_script.hook("function", nullptr, res);

    }




    {
        std::string p, c;

        int ret = Script::getScriptDetails(MapType(),
                                                    "7d7a418d-52b0-4961-96cb-6030c4f8666a", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = 0x48f9;
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = "foo.bar";
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = "foo.bar";
        script["language"] = 0x8c39;
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = "foo.bar";
        script["language"] = "ruby"; // not python
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = "foo.bar";
        script["language"] = "python";
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == 0);

        assert(p == "foo");
        assert(c == "bar");
    }

    {
        std::string p, c;

        MapType script;
        script["name"] = "foo";
        script["language"] = "python";
        int ret = Script::getScriptDetails(script,
                                                    "", "RuleHandlertest", p, c);
        assert(ret == -1);

        assert(p.empty());
        assert(c.empty());
    }
}

#include "../stubs/common/stublog.h"
