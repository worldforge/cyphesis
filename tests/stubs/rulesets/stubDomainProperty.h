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
#ifndef STUBDOMAINPROPERTY_H_
#define STUBDOMAINPROPERTY_H_

#include "rulesets/DomainProperty.h"

DomainProperty::DomainProperty()
{
}

DomainProperty::DomainProperty(const DomainProperty& rhs)
{
}

void DomainProperty::install(LocatedEntity *entity, const std::string &)
{
}

void DomainProperty::remove(LocatedEntity * entity, const std::string &)
{
}

void DomainProperty::apply(LocatedEntity * entity)
{
}

DomainProperty * DomainProperty::copy() const
{
    return nullptr;
}

Domain* DomainProperty::getDomain(const LocatedEntity *) const {
    return nullptr;
}




#endif /* STUBDOMAINPROPERTY_H_ */
