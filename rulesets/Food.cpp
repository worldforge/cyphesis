// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Food.h"
#include "Script.h"

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Nourish.h"

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Set.h>

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

OpVector Food::EatOperation(const Eat & op)
{
    OpVector res;
    if (m_script->Operation("eat", op, res) != 0) {
        return res;
    }
    Element::MapType self_ent;
    self_ent["id"] = getId();
    self_ent["status"] = -1;

    Set * s = new Set();
    s->setTo(getId());
    s->setArgs(Element::ListType(1,self_ent));

    const std::string & to = op.getFrom();
    Element::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["mass"] = m_mass;
    Nourish * n = new Nourish();
    n->setTo(to);
    n->setArgs(Element::ListType(1,nour_ent));

    OpVector res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

OpVector Food::BurnOperation(const Burn & op)
{
    OpVector res;
    if (m_script->Operation("burn", op, res) != 0) {
        return res;
    }
    double cooked = 0;
    if (op.getArgs().empty() || !op.getArgs().front().isMap()) {
       return error(op, "Fire op has no argument", getId());
    }
    Element::MapType::const_iterator I = m_attributes.find("cooked");
    if ((I != m_attributes.end()) && I->second.isNum()) {
        cooked = I->second.asNum();
    }
    const Element::MapType & fire_ent = op.getArgs().front().asMap();
    Element::MapType self_ent;
    self_ent["id"] = getId();
    // Currently this cooks pretty quick, and at the same speed for
    // everything. No mechanism for this yet.
    double fire_size = fire_ent.find("status")->second.asNum();
    self_ent["cooked"] = cooked + (fire_size/m_mass);
    if (cooked > 1.0) {
        self_ent["status"] = m_status - (m_attributes["burn_speed"].asNum()) * fire_size;
    }

    Set * s = new Set();
    s->setTo(getId());
    s->setArgs(Element::ListType(1,self_ent));

    return OpVector(1,s);
}

