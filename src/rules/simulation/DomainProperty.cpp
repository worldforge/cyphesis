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
#include "rules/LocatedEntity.h"
#include "StackableDomain.h"
#include "ContainerDomain.h"


#include <Atlas/Objects/Anonymous.h>
#include <rules/simulation/BaseWorld.h>


void DomainProperty::install(LocatedEntity& entity, const std::string& name)
{
}

void DomainProperty::remove(LocatedEntity& entity, const std::string& name)
{
    if (entity.getDomain()) {
        entity.setDomain(nullptr);
    }
}

void DomainProperty::apply(LocatedEntity& entity)
{
    if (!m_data.empty()) {
        if (!entity.getDomain()) {
            std::unique_ptr<Domain> domain;
            if (m_data == "physical") {
                domain = std::make_unique<PhysicalDomain>(entity);
            } else if (m_data == "void") {
                domain = std::make_unique<VoidDomain>(entity);
            } else if (m_data == "inventory") {
                domain = std::make_unique<InventoryDomain>(entity);
            } else if (m_data == "stackable") {
                domain = std::make_unique<StackableDomain>(entity);
            } else if (m_data == "container") {
                domain = std::make_unique<ContainerDomain>(entity);
            }
            if (domain) {
                domain->installDelegates(entity, property_name);
            }
            entity.setDomain(std::move(domain));
        }
    } else {
        entity.setDomain(nullptr);
    }
}

DomainProperty* DomainProperty::copy() const
{
    return new DomainProperty(*this);
}

HandlerResult DomainProperty::operation(LocatedEntity& e, const Operation& op, OpVector& res)
{
    return e.getDomain()->operation(e, op, res);
}



