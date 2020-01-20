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
#include <Atlas/Objects/Factories.h>

using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;

static TypeNode * stub_addChild_result = 0;
Atlas::Objects::Factories factories;

int main()
{
    Inheritance inheritance(factories);
    {
        RuleHandler * rh = new OpRuleHandler();
        delete rh;
    }

    // check() empty description
    {
        RuleHandler * rh = new OpRuleHandler();

        Anonymous description;
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == -1);

        delete rh;
    }

    // check() description with op_definition objtype
    {
        RuleHandler * rh = new OpRuleHandler();

        Anonymous description;
        description->setObjtype("op_definition");
        description->setParent("foo");
        int ret = rh->check(description);

        assert(ret == 0);

        delete rh;
    }

    {
        RuleHandler * rh = new OpRuleHandler();

        Anonymous description;
        std::string dependent, reason;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

        int ret = rh->install("", "", description, dependent, reason, changes);

        assert(ret == -1);

        delete rh;
    }

    // Install a rule with addChild rigged to give a correct result
    {
        RuleHandler * rh = new OpRuleHandler();

        Anonymous description;
        std::string dependent, reason;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

        stub_addChild_result = (TypeNode *) malloc(sizeof(TypeNode));
        int ret = rh->install("", "", description, dependent, reason, changes);

        assert(ret == 0);

        free(stub_addChild_result);
        stub_addChild_result = 0;

        delete rh;
    }
    {
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
        RuleHandler * rh = new OpRuleHandler();

        Anonymous description;
        int ret = rh->update("", description, changes);

        // FIXME Currently does nothing
        assert(ret == 0);

        delete rh;
    }

 
}

// stubs

#include "common/Inheritance.h"
#include "common/log.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubProperty.h"

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

#include "../stubs/common/stubInheritance.h"


Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    return Root();
}
#include "../stubs/common/stublog.h"
