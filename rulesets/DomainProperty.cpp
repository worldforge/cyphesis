/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H

#endif

#include "DomainProperty.h"
#include "PhysicalDomain.h"
#include "VoidDomain.h"
#include "InventoryDomain.h"
#include "LocatedEntity.h"

#include "common/const.h"
#include "common/Tick.h"

#include <Atlas/Objects/Anonymous.h>
#include <common/BaseWorld.h>

const std::string DomainProperty::property_name = "domain";
const std::string DomainProperty::property_atlastype = "string";

PropertyInstanceState<Domain> DomainProperty::sInstanceState;

DomainProperty::DomainProperty()
{
}

DomainProperty::DomainProperty(const DomainProperty& rhs) :
    Property(rhs)
{
}

void DomainProperty::install(LocatedEntity* entity, const std::string& name)
{
    sInstanceState.addState(entity, nullptr);

    entity->installDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

void DomainProperty::remove(LocatedEntity* entity, const std::string& name)
{
    sInstanceState.removeState(entity);
    entity->setFlags(~entity_domain);
    entity->removeDelegate(Atlas::Objects::Operation::TICK_NO, name);
}

void DomainProperty::apply(LocatedEntity* entity)
{
    if (m_data != "") {
        Domain* domain = sInstanceState.getState(entity);
        if (!domain) {
            if (m_data == "physical") {
                domain = new PhysicalDomain(*entity);
                sInstanceState.replaceState(entity, domain);
                entity->setFlags(entity_domain);
                OpVector res;
                double tickSize = 1.0 / 15.0;
                domain->tick(tickSize, res);
                for (auto& op : res) {
                    entity->sendWorld(op);
                }
                scheduleTick(*entity, BaseWorld::instance().getTime());
            } else if (m_data == "void") {
                domain = new VoidDomain(*entity);
                sInstanceState.replaceState(entity, domain);
                entity->setFlags(entity_domain);
            } else if (m_data == "inventory") {
                domain = new InventoryDomain(*entity);
                sInstanceState.replaceState(entity, domain);
                entity->setFlags(entity_domain);
            }
        }
    } else {
        sInstanceState.replaceState(entity, nullptr);
        entity->setFlags(~entity_domain);
    }
}

DomainProperty* DomainProperty::copy() const
{
    return new DomainProperty(*this);
}

Domain* DomainProperty::getDomain(const LocatedEntity* entity) const
{
    return sInstanceState.getState(entity);
}

void DomainProperty::scheduleTick(LocatedEntity& entity, double timeNow)
{
    Atlas::Objects::Entity::Anonymous tick_arg;
    tick_arg->setName("domain");
    Atlas::Objects::Operation::Tick tickOp;
    tickOp->setTo(entity.getId());
    tickOp->setSeconds(timeNow + ((1.0 / 15.0) / consts::time_multiplier));
    tickOp->setAttr("lastTick", timeNow);
    tickOp->setArgs1(tick_arg);

    entity.sendWorld(tickOp);
}

HandlerResult DomainProperty::operation(LocatedEntity* e, const Operation& op, OpVector& res)
{
    return tick_handler(e, op, res);
}

HandlerResult DomainProperty::tick_handler(LocatedEntity* entity, const Operation& op, OpVector& res)
{
    if (!op->getArgs().empty() && op->getArgs().front()->getName() == "domain") {
        Domain* domain = sInstanceState.getState(entity);
        if (domain) {

            double timeNow = op->getSeconds();
            double tickSize = 1.0 / 15.0;
            Atlas::Message::Element elem;
            if (op->copyAttr("lastTick", elem) != 0 && elem.isFloat()) {
                tickSize = timeNow - elem.Float();

            }

            domain->tick(tickSize, res);
            scheduleTick(*entity, timeNow);
        }
        return OPERATION_BLOCKED;
    }
    return OPERATION_IGNORED;
}

