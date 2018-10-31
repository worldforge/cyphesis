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


#include "BurnSpeedProperty.h"

#include "rules/LocatedEntity.h"
#include "StatusProperty.h"

#include "common/debug.h"

#include "common/operations/Burn.h"
#include "common/operations/Nourish.h"
#include "common/operations/Update.h"

#include <Atlas/Objects/Anonymous.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Root;

static const bool debug_flag = false;

void BurnSpeedProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::BURN_NO, name);
}

void BurnSpeedProperty::remove(LocatedEntity *owner, const std::string & name)
{
    owner->removeDelegate(Atlas::Objects::Operation::BURN_NO, name);
}

HandlerResult BurnSpeedProperty::operation(LocatedEntity * ent,
                                        const Operation & op,
                                        OpVector & res)
{
    return BurnSpeedProperty::burn_handler(ent, op, res);
}

BurnSpeedProperty * BurnSpeedProperty::copy() const
{
    return new BurnSpeedProperty(*this);
}

HandlerResult BurnSpeedProperty::burn_handler(LocatedEntity * e,
                                              const Operation & op,
                                              OpVector & res)
{
    if (op->getArgs().empty()) {
        e->error(op, "Fire op has no argument", res, e->getId());
        return OPERATION_IGNORED;
    }

    const double & burn_speed = data();
    const Root & fire_ent = op->getArgs().front();
    double consumed = burn_speed * fire_ent->getAttr("status").asNum();

    const std::string & to = fire_ent->getId();
    Anonymous nour_ent;
    nour_ent->setId(to);
    nour_ent->setAttr("mass", consumed);

    StatusProperty * status_prop = e->requirePropertyClassFixed<StatusProperty>(1.f);
    assert(status_prop != 0);
    status_prop->addFlags(flag_unsent);
    double & status = status_prop->data();

    Element mass_attr;
    if (e->getAttrType("mass", mass_attr, Element::TYPE_FLOAT) != 0) {
        mass_attr = 1.f;
    }
    status -= (consumed / mass_attr.Float());
    status_prop->apply(e);

    Update update;
    update->setTo(e->getId());
    res.push_back(update);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_ent);

    res.push_back(n);

    return OPERATION_IGNORED;
}

