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
#include "config.h"
#endif

#include "DomainProperty.h"
#include "Domain.h"
#include "LocatedEntity.h"

DomainProperty::DomainProperty()
: m_domain(nullptr)
{
}

DomainProperty::DomainProperty(const DomainProperty& rhs)
: m_domain(nullptr)
{
}

void DomainProperty::remove(LocatedEntity * entity, const std::string &)
{
    delete m_domain;
    entity->setFlags(~entity_domain);
}

void DomainProperty::apply(LocatedEntity * entity)
{
    if (m_data) {
        if (!m_domain) {
            m_domain = new Domain();
        }
        entity->setFlags(entity_domain);
    } else {
        delete m_domain;
        m_domain = nullptr;
        entity->setFlags(~entity_domain);
    }
}

DomainProperty * DomainProperty::copy() const
{
    return new DomainProperty(*this);
}

Domain* DomainProperty::getDomain() const {
    return m_domain;
}

