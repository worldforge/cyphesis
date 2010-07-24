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

// $Id$

#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>

using Atlas::Objects::Root;

int main()
{
    installCustomOperations();
    installCustomEntities();
    return 0;
}

// stubs

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

TypeNode * Inheritance::addChild(const Root & obj,
                                 const PropertyDict & defaults)
{
    return 0;
}

Root atlasOpDefinition(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("op_definition");
    r->setId(name);

    return r;
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;

    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("class");
    r->setId(name);

    return r;
}


namespace Atlas { namespace Objects { namespace Operation {

int ACTUATE_NO = -1;
int ADD_NO = -1;
int ATTACK_NO = -1;
int BURN_NO = -1;
int CONNECT_NO = -1;
int DROP_NO = -1;
int MONITOR_NO = -1;
int EAT_NO = -1;
int UNSEEN_NO = -1;
int UPDATE_NO = -1;
int NOURISH_NO = -1;
int PICKUP_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int THOUGHT_NO = -1;
int GOAL_INFO_NO = -1;
int TELEPORT_NO = -1;

} } }
