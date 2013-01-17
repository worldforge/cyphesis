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

#include "rulesets/BurnSpeedProperty.h"

#include "rulesets/LocatedEntity.h"
#include "rulesets/StatusProperty.h"

#include "common/debug.h"

#include "common/Nourish.h"
#include "common/Update.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Root;

static const bool debug_flag = false;

HandlerResult BurnSpeedProperty::burn_handler(LocatedEntity * e,
                                              const Operation & op,
                                              OpVector & res)
{
    if (op->getArgs().empty()) {
        e->error(op, "Fire op has no argument", res, e->getId());
        return OPERATION_IGNORED;
    }

    const Property<double> * pb = e->getPropertyType<double>("burn_speed");
    if (pb == NULL) {
        debug(std::cout << "Eat HANDLER no burn_speed" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    
    const double & burn_speed = pb->data();
    const Root & fire_ent = op->getArgs().front();
    double consumed = burn_speed * fire_ent->getAttr("status").asNum();

    const std::string & to = fire_ent->getId();
    Anonymous nour_ent;
    nour_ent->setId(to);
    nour_ent->setAttr("mass", consumed);

    StatusProperty * status_prop = e->requirePropertyClass<StatusProperty>("status", 1.f);
    assert(status_prop != 0);
    status_prop->setFlags(flag_unsent);
    double & status = status_prop->data();

    Element mass_attr;
    if (e->getAttrType("mass", mass_attr, Element::TYPE_FLOAT) != 0) {
        mass_attr = 1.f;
    }
    status -= (consumed / mass_attr.Float());

    Update update;
    update->setTo(e->getId());
    res.push_back(update);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_ent);

    res.push_back(n);

    return OPERATION_IGNORED;
}

