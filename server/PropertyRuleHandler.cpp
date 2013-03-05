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

#include "PropertyRuleHandler.h"

#include "EntityBuilder.h"
#include "TaskFactory.h"

#include "rulesets/PythonScriptFactory.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/Inheritance.h"
#include "common/compose.hpp"
#include "common/PropertyManager.h"
#include "common/PropertyFactory.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;

using String::compose;

static const bool debug_flag = false;

int PropertyRuleHandler::check(const Atlas::Objects::Root & desc)
{
    assert(!desc->getParents().empty());
    if (desc->getObjtype() != "type") {
        return -1;
    }
    return 0;
}

int PropertyRuleHandler::install(const std::string & name,
                                 const std::string & parent,
                                 const Atlas::Objects::Root & desc,
                                 std::string & dependent,
                                 std::string & reason)
{
    assert(desc->getObjtype() == "type");
    PropertyManager * pm = PropertyManager::instance();
    if (pm->getPropertyFactory(name) != 0) {
        log(ERROR, compose("Property rule \"%1\" already exists.", name));
        return -1;
    }
    PropertyKit * parent_factory = pm->getPropertyFactory(parent);
    if (parent_factory == 0) {
        dependent = parent;
        reason = compose("Property rule \"%1\" has parent \"%2\" which does "
                         "not exist.", name, parent);
        return 1;
    }

    Element script_attr;
    if (desc->copyAttr("script", script_attr) == 0 && script_attr.isMap()) {
        log(NOTICE, compose("Property \"%1\" has a script.", name));

        const MapType & script = script_attr.Map();
        std::string script_package;
        std::string script_class;

        if (getScriptDetails(script, name, "Task",
                             script_package, script_class) != 0) {
            log(NOTICE, compose("Property \"%1\" has a malformed script.", name));
        }
    }

    auto * factory = parent_factory->duplicateFactory();
    assert(factory != 0);
    pm->installFactory(name, desc, factory);
    return 0;
}

int PropertyRuleHandler::update(const std::string & name,
                                const Atlas::Objects::Root & desc)
{
    // There is not anything to be modified yet.
    return 0;
}
