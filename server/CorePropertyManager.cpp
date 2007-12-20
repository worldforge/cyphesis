// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id: CorePropertyManager.cpp,v 1.27 2007-12-20 18:47:29 alriddoch Exp $

#include "CorePropertyManager.h"

#include "rulesets/ActivePropertyFactory_impl.h"
#include "rulesets/EntityPropertyFactory_impl.h"

#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Entity.h"

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Setup.h"

#include "common/types.h"
#include "common/PropertyFactory_impl.h"
#include "common/DynamicProperty_impl.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <wfmath/atlasconv.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Message::Element;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

template class PropertyFactory<Dynamic<LineProperty, CoordList> >;

HandlerResult test_handler(Entity *, const Operation &, OpVector & res)
{
    debug(std::cout << "TEST HANDLER CALLED" << std::endl << std::flush;);
    return OPERATION_IGNORED;
}

HandlerResult del_handler(Entity * e, const Operation &, OpVector & res)
{
    debug(std::cout << "Delete HANDLER CALLED" << std::endl << std::flush;);
    PropertyBase * pb = e->getProperty("decays");
    if (pb == NULL) {
        debug(std::cout << "Delete HANDLER no decays" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    Element val;
    pb->get(val);
    if (!val.isString()) {
        debug(std::cout << "Delete HANDLER decays non-string" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    const std::string & type = val.String();

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

HandlerResult eat_handler(Entity * e, const Operation & op, OpVector & res)
{
    PropertyBase * pb = e->getProperty("biomass");
    if (pb == NULL) {
        debug(std::cout << "Eat HANDLER no biomass" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    
    Element val;
    pb->get(val);
    if (!val.isNum()) {
        debug(std::cout << "Eat HANDLER biomass non-float" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    double biomass = val.asNum();

    Anonymous self;
    self->setId(e->getId());
    self->setAttr("status", -1);

    Set s;
    s->setTo(e->getId());
    s->setArgs1(self);

    const std::string & to = op->getFrom();
    Anonymous nour_arg;
    nour_arg->setId(to);
    nour_arg->setAttr("mass", biomass);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_arg);

    res.push_back(s);
    res.push_back(n);

    return OPERATION_IGNORED;
}

HandlerResult burn_handler(Entity * e, const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        e->error(op, "Fire op has no argument", res, e->getId());
        return OPERATION_IGNORED;
    }

    PropertyBase * pb = e->getProperty("burn_speed");
    if (pb == NULL) {
        debug(std::cout << "Eat HANDLER no burn_speed" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    
    Element val;
    pb->get(val);
    if (!val.isNum()) {
        debug(std::cout << "Burn HANDLER burn_speed non-float" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }

    double burn_speed = val.asNum();
    const Root & fire_ent = op->getArgs().front();
    double consumed = burn_speed * fire_ent->getAttr("status").asNum();

    const std::string & to = fire_ent->getId();
    Anonymous nour_ent;
    nour_ent->setId(to);
    nour_ent->setAttr("mass", consumed);

    Set s;
    s->setTo(e->getId());

    Element new_status;
    PropertyBase * status = e->getProperty("status");
    if (status == 0 || !status->get(new_status) || new_status.isNum()) {
        new_status = 1.f;
    }
    Element mass_attr(1.f);
    e->getAttr("mass", mass_attr);
    if (!mass_attr.isFloat()) {
        mass_attr = 1.f;
    }
    new_status = new_status.asNum() - (consumed / mass_attr.Float());
    Anonymous self_ent;
    self_ent->setId(e->getId());
    self_ent->setAttr("status", new_status);
    s->setArgs1(self_ent);
    
    res.push_back(s);

    Nourish n;
    n->setTo(to);
    n->setArgs1(nour_ent);

    res.push_back(n);

    return OPERATION_IGNORED;
}

HandlerResult transient_handler(Entity * e,
                                const Operation & op,
                                OpVector & res)
{
    Element transient_time;
    if (!e->getAttr("transient", transient_time)) {
        return OPERATION_IGNORED;
    }

    if (!transient_time.isFloat()) {
        return OPERATION_IGNORED;
    }

    Set s;
    s->setTo(e->getId());
    s->setFutureSeconds(transient_time.Float());

    Anonymous set_arg;
    set_arg->setId(e->getId());
    set_arg->setAttr("status", -1);
    s->setArgs1(set_arg);

    res.push_back(s);

    return OPERATION_IGNORED;
}

CorePropertyManager::CorePropertyManager()
{
    m_propertyFactories["stamina"] = new PropertyFactory<DynamicProperty<double> >;
    m_propertyFactories["coords"] = new PropertyFactory<Dynamic<LineProperty, CoordList> >;
    m_propertyFactories["points"] = new PropertyFactory<Dynamic<LineProperty, CoordList> >;
    m_propertyFactories["start_intersections"] = new PropertyFactory<DynamicProperty<IdList> >;
    m_propertyFactories["end_intersections"] = new PropertyFactory<DynamicProperty<IdList> >;
    m_propertyFactories["attachment"] = new ActivePropertyFactory<DynamicProperty<int> >(Atlas::Objects::Operation::MOVE_NO, test_handler);
    m_propertyFactories["decays"] = new ActivePropertyFactory<DynamicProperty<std::string> >(Atlas::Objects::Operation::DELETE_NO, del_handler);
    m_propertyFactories["outfit"] = new PropertyFactory<OutfitProperty>;
    m_propertyFactories["solid"] = new EntityPropertyFactory<SolidProperty>;
    m_propertyFactories["status"] = new EntityPropertyFactory<StatusProperty>;
    m_propertyFactories["biomass"] = new ActivePropertyFactory<DynamicProperty<double> >(Atlas::Objects::Operation::EAT_NO, eat_handler);
    m_propertyFactories["burn_speed"] = new ActivePropertyFactory<DynamicProperty<double> >(Atlas::Objects::Operation::BURN_NO, burn_handler);
    m_propertyFactories["transient"] = new ActivePropertyFactory<DynamicProperty<double> >(Atlas::Objects::Operation::SETUP_NO, transient_handler);
    m_propertyFactories["food"] = new PropertyFactory<DynamicProperty<double> >;
    m_propertyFactories["mass"] = new PropertyFactory<DynamicProperty<double> >;
}

CorePropertyManager::~CorePropertyManager()
{
    std::map<std::string, PropertyKit *>::const_iterator I = m_propertyFactories.begin();
    std::map<std::string, PropertyKit *>::const_iterator Iend = m_propertyFactories.end();
    for (; I != Iend; ++I) {
        assert(I->second != 0);
        delete I->second;
    }
}

PropertyBase * CorePropertyManager::addProperty(Entity * entity,
                                                const std::string & name)
{
    assert(entity != 0);
    assert(!name.empty());
    PropertyFactoryDict::const_iterator I = m_propertyFactories.find(name);
    if (I == m_propertyFactories.end()) {
        return 0;
    }
    debug(std::cout << name << " property found. " << entity->getId() << std::endl << std::flush;);
    PropertyBase * p = I->second->newProperty(entity);
    return p;
}
