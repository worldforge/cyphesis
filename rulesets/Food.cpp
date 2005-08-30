// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Food.h"
#include "Script.h"

#include "common/Nourish.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Entity::Anonymous;

Food::Food(const std::string & id) : Food_parent(id)
{
    m_attributes["cooked"] = 0;
    m_attributes["burn_speed"] = 0.1;
    m_mass = 1;

    subscribe("eat", OP_EAT);
}

Food::~Food()
{
}

void Food::EatOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("eat", op, res) != 0) {
        return;
    }

    Anonymous self;
    self->setId(getId());
    self->setAttr("status", -1);

    Set s;
    s->setTo(getId());
    s->setArgs1(self);

    const std::string & to = op->getFrom();
    Anonymous nour_arg;
    nour_arg->setId(to);
    nour_arg->setAttr("mass", m_mass);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_arg);

    res.push_back(s);
    res.push_back(n);
}

void Food::BurnOperation(const Operation & op, OpVector & res)
{
    if (m_script->Operation("burn", op, res) != 0) {
        return;
    }
    if (op->getArgs().empty()) {
       error(op, "Burn op has no argument", res, getId());
       return;
    }
    double cooked = 0;
    Element cooked_attr;
    if (get("cooked", cooked_attr) && cooked_attr.isNum()) {
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

