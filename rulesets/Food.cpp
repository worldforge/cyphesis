// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: Food.cpp,v 1.45 2007-06-21 20:26:53 alriddoch Exp $

#include "Food.h"
#include "Script.h"

#include "common/Eat.h"
#include "common/Nourish.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Entity::Anonymous;

Food::Food(const std::string & id, long intId) : Food_parent(id, intId)
{
    m_attributes["cooked"] = 0;
    m_attributes["burn_speed"] = 0.1;
    m_mass = 1;
}

Food::~Food()
{
}

void Food::BurnOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
       error(op, "Burn op has no argument", res, getId());
       return;
    }
    double cooked = 0;
    Element cooked_attr;
    if (getAttr("cooked", cooked_attr) && cooked_attr.isNum()) {
        cooked = cooked_attr.asNum();
    }
    const Root & arg = op->getArgs().front();
    Anonymous set_arg;
    set_arg->setId(getId());
    // Currently this cooks pretty quick, and at the same speed for
    // everything. No mechanism for this yet.
    Element status_attr;
    if (arg->copyAttr("status", status_attr) != 0 || !status_attr.isNum()) {
        error(op, "Burn op with no fire status", res, getId());
    } else {
        double fire_size = status_attr.asNum();
        set_arg->setAttr("cooked", cooked + (fire_size/m_mass));
        if (cooked > 1.0) {
            set_arg->setAttr("status", m_status - (m_attributes["burn_speed"].asNum()) * fire_size);
        }
    }

    Set s;
    s->setTo(getId());
    s->setArgs1(set_arg);

    res.push_back(s);
}

