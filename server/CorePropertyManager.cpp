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

// $Id$

#include "CorePropertyManager.h"

#include "ExternalMind.h"
#include "ServerRouting.h"
#include "Peer.h"

#include "rulesets/ActivePropertyFactory_impl.h"

#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/SolidProperty.h"
#include "rulesets/StatusProperty.h"
#include "rulesets/Entity.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TransientProperty.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/MindProperty.h"
#include "rulesets/InternalProperties.h"
#include "rulesets/SpawnProperty.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/Character.h"

#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Nourish.h"
#include "common/Update.h"
#include "common/Teleport.h"

#include "common/types.h"
#include "common/PropertyFactory_impl.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <wfmath/atlasconv.h>
#include <wfmath/MersenneTwister.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Message::Element;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Burn;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Delete;
using Atlas::Objects::Operation::Logout;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Update;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

HandlerResult test_handler(Entity *, const Operation &, OpVector & res)
{
    debug(std::cout << "TEST HANDLER CALLED" << std::endl << std::flush;);
    return OPERATION_IGNORED;
}

HandlerResult del_handler(Entity * e, const Operation &, OpVector & res)
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

HandlerResult eat_handler(Entity * e, const Operation & op, OpVector & res)
{
    const Property<double> * pb = e->getPropertyType<double>("biomass");
    if (pb == NULL) {
        debug(std::cout << "Eat HANDLER no biomass" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }
    
    const double & biomass = pb->data();

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
    if (!e->getAttrType("mass", mass_attr, Element::TYPE_FLOAT)) {
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

HandlerResult terrainmod_moveHandler(Entity * e,
                                 const Operation & op,
                                 OpVector & res)
{
    TerrainModProperty * mod_property = e->modPropertyClass<TerrainModProperty>("terrainmod");
    if (mod_property == 0) {
        return OPERATION_IGNORED;
    }

    // Check the validity of the operation.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return OPERATION_IGNORED;
    }
    RootEntity ent = Atlas::Objects::smart_dynamic_cast<RootEntity>(args.front());
    if (!ent.isValid()) {
        return OPERATION_IGNORED;
    }
    if (e->getId() != ent->getId()) {
        return OPERATION_IGNORED;
    }

    Point3D newPos(ent->getPos()[0], ent->getPos()[1], ent->getPos()[2]);

    // If we have any terrain mods applied, remove them from the previous pos and apply them to the new one
    mod_property->move(e, newPos);
    return OPERATION_IGNORED;
}

HandlerResult terrainmod_deleteHandler(Entity * e,
                                 const Operation & op,
                                 OpVector & res)
{
    TerrainModProperty * mod_property = e->modPropertyClass<TerrainModProperty>("terrainmod");
    if (mod_property == 0) {
        return OPERATION_IGNORED;
    }

    mod_property->remove(e);

    return OPERATION_IGNORED;
}

HandlerResult teleport_handler(Entity * e, const Operation & op, OpVector & res)
{
    // Get the teleport property value (in our case, the IP to teleport to)
    const Property<std::string> * pb = e->getPropertyType<std::string>("teleport");
    if (pb == NULL) {
        debug(std::cout << "Teleport HANDLER no teleport" << std::endl 
                        << std::flush;);
        return OPERATION_IGNORED;
    }

    ServerRouting *svr = ServerRouting::instance();
    if(svr == NULL) {
        log(ERROR, "Unable to access ServerRouting object");
        return OPERATION_IGNORED;
    }
    // Try and get the peer with the requested ID
    Peer *peer = dynamic_cast<Peer*>(svr->getObject(pb->data()));
    if(peer == NULL) {
        log(ERROR, "Unknown or disconnected peer ID specified");
        return OPERATION_IGNORED;
    }

    // Get the ID of the sender
    const std::string & from = op->getFrom();
    if (from.empty()) {
        debug(std::cout << "ERROR: Operation with no entity to be teleported" 
                        << std::endl << std::flush;);
        return OPERATION_IGNORED;
    }
    log(INFO, String::compose("Teleport request sender has ID %1", from));

    // This is the sender entity
    Entity * entity = BaseWorld::instance().getEntity(from);
    if (entity == 0) {
        debug(std::cout << "No entity found with the specified ID: "
                        << from;);
        return OPERATION_IGNORED;
    }

    // Get an Atlas representation and inject it on remote server
    Atlas::Objects::Entity::Anonymous atlas_repr;
    entity->addToEntity(atlas_repr);
    peer->teleportEntity(atlas_repr);
    return OPERATION_IGNORED;
}

CorePropertyManager::CorePropertyManager()
{
    m_propertyFactories["stamina"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["coords"] = new PropertyFactory<LineProperty>;
    m_propertyFactories["points"] = new PropertyFactory<LineProperty>;
    m_propertyFactories["start_intersections"] = new PropertyFactory<Property<IdList> >;
    m_propertyFactories["end_intersections"] = new PropertyFactory<Property<IdList> >;
    m_propertyFactories["attachment"] = new ActivePropertyFactory<int>(Atlas::Objects::Operation::MOVE_NO, test_handler);
    m_propertyFactories["decays"] = new ActivePropertyFactory<std::string>(Atlas::Objects::Operation::DELETE_NO, del_handler);
    m_propertyFactories["outfit"] = new PropertyFactory<OutfitProperty>;
    m_propertyFactories["solid"] = new PropertyFactory<SolidProperty>;
    m_propertyFactories["simple"] = new PropertyFactory<SimpleProperty>;
    m_propertyFactories["status"] = new PropertyFactory<StatusProperty>;
    m_propertyFactories["biomass"] = new ActivePropertyFactory<double>(Atlas::Objects::Operation::EAT_NO, eat_handler);
    m_propertyFactories["burn_speed"] = new ActivePropertyFactory<double>(Atlas::Objects::Operation::BURN_NO, burn_handler);
    m_propertyFactories["transient"] = new PropertyFactory<TransientProperty>();
    m_propertyFactories["food"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["mass"] = new PropertyFactory<Property<double> >;
    m_propertyFactories["bbox"] = new PropertyFactory<BBoxProperty>;
    m_propertyFactories["mind"] = new PropertyFactory<MindProperty>;
    m_propertyFactories["setup"] = new PropertyFactory<SetupProperty>;
    m_propertyFactories["tick"] = new PropertyFactory<TickProperty>;
    m_propertyFactories["statistics"] = new PropertyFactory<StatisticsProperty>;
    m_propertyFactories["spawn"] = new PropertyFactory<SpawnProperty>;
    m_propertyFactories["area"] = new PropertyFactory<AreaProperty>;
    
    HandlerMap terrainModHandles;
    terrainModHandles[Atlas::Objects::Operation::MOVE_NO] = terrainmod_moveHandler;
    terrainModHandles[Atlas::Objects::Operation::DELETE_NO] = terrainmod_deleteHandler;
    m_propertyFactories["terrainmod"] = new MultiActivePropertyFactory<TerrainModProperty>(terrainModHandles);

    m_propertyFactories["teleport"] = new ActivePropertyFactory<std::string>(Atlas::Objects::Operation::TELEPORT_NO, teleport_handler);
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

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    assert(!name.empty());
    assert(name != "objtype");
    PropertyBase * p = 0;
    PropertyFactoryDict::const_iterator I = m_propertyFactories.find(name);
    if (I == m_propertyFactories.end()) {
        switch (type) {
          case Element::TYPE_INT:
            p = new Property<int>;
            break;
          case Element::TYPE_FLOAT:
            p = new Property<double>;
            break;
          case Element::TYPE_STRING:
            p = new Property<std::string>;
            break;
          default:
            p = new SoftProperty;
            break;
        }
    } else {
        p = I->second->newProperty();
    }
    debug(std::cout << name << " property found. " << std::endl << std::flush;);
    return p;
}
