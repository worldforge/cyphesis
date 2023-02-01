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

#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Factories.h>

using Atlas::Objects::Root;

namespace {
    Atlas::Objects::Factories factories;
}

int main()
{
    Inheritance inheritance(factories);
    installCustomOperations(inheritance);
    installCustomEntities(inheritance);
    return 0;
}

// stubs
#include "../stubs/common/stubInheritance.h"

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParent(parent);
    r->setObjtype("op_definition");
    r->setId(name);

    return r;
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParent(parent);
    r->setObjtype("class");
    r->setId(name);

    return r;
}

#include "../stubs/common/stubcustom.h"


#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubProperty.h"
