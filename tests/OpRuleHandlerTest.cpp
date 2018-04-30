// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "server/OpRuleHandler.h"

#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdlib>
#include <common/Inheritance.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

static TypeNode * stub_addChild_result = 0;

int main()
{
    Inheritance inheritance;
    {
        RuleHandler * rh = new OpRuleHandler(0);
        delete rh;
    }

    // check() empty description
    {
        RuleHandler * rh = new OpRuleHandler(0);

        Anonymous description;
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == -1);

        delete rh;
    }

    // check() description with op_definition objtype
    {
        RuleHandler * rh = new OpRuleHandler(0);

        Anonymous description;
        description->setObjtype("op_definition");
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == 0);

        delete rh;
    }

    {
        RuleHandler * rh = new OpRuleHandler(0);

        Anonymous description;
        std::string dependent, reason;
        int ret = rh->install("", "", description, dependent, reason);

        assert(ret == -1);

        delete rh;
    }

    // Install a rule with addChild rigged to give a correct result
    {
        RuleHandler * rh = new OpRuleHandler(0);

        Anonymous description;
        std::string dependent, reason;

        stub_addChild_result = (TypeNode *) malloc(sizeof(TypeNode));
        int ret = rh->install("", "", description, dependent, reason);

        assert(ret == 0);

        free(stub_addChild_result);
        stub_addChild_result = 0;

        delete rh;
    }
    {
        RuleHandler * rh = new OpRuleHandler(0);

        Anonymous description;
        int ret = rh->update("", description);

        // FIXME Currently does nothing
        assert(ret == 0);

        delete rh;
    }

 
}

// stubs

#include "common/Inheritance.h"
#include "common/log.h"


int RuleHandler::getScriptDetails(const Atlas::Message::MapType & script,
                                  const std::string & class_name,
                                  const std::string & context,
                                  std::string & script_package,
                                  std::string & script_class)
{
    return 0;
}


#ifndef STUB_Inheritance_addChild
#define STUB_Inheritance_addChild
TypeNode* Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    return stub_addChild_result;
}
#endif //STUB_Inheritance_addChild


#ifndef STUB_Inheritance_hasClass
#define STUB_Inheritance_hasClass
bool Inheritance::hasClass(const std::string & parent)
{
    return true;
}
#endif //STUB_Inheritance_hasClass

#include "stubs/common/stubInheritance.h"


Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    return Root();
}

void log(LogLevel lvl, const std::string & msg)
{
}
