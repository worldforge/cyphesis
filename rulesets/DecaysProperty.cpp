// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "rulesets/DecaysProperty.h"

#include "rulesets/LocatedEntity.h"

#include "common/debug.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <wfmath/atlasconv.h>

#include <iostream>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;

static const bool debug_flag = false;

HandlerResult DecaysProperty::del_handler(LocatedEntity * e,
                                          const Operation &,
                                          OpVector & res)
{
    debug(std::cout << "Delete HANDLER CALLED" << std::endl << std::flush;);
    const Property<std::string> * pb = e->getPropertyType<std::string>("decays");
    if (pb == NULL) {
        debug(std::cout << "Delete HANDLER no decays" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    const std::string & type = pb->data();

    Anonymous create_arg;
    create_arg->setParents(std::list<std::string>(1, type));
    ::addToEntity(e->m_location.pos(), create_arg->modifyPos());
    create_arg->setLoc(e->m_location.m_loc->getId());
    create_arg->setAttr("orientation", e->m_location.orientation().toAtlas());

    Create create;
    create->setTo(e->m_location.m_loc->getId());
    create->setArgs1(create_arg);
    res.push_back(create);

    return OPERATION_IGNORED;
}
