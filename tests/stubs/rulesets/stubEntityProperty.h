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
#ifndef STUBENTITYPROPERTY_H_
#define STUBENTITYPROPERTY_H_


EntityProperty::EntityProperty()
{
}

int EntityProperty::get(Atlas::Message::Element & val) const
{
    return 0;
}

void EntityProperty::set(const Atlas::Message::Element & val)
{
}

void EntityProperty::add(const std::string & s,
                         Atlas::Message::MapType & map) const
{
}

void EntityProperty::add(const std::string & s,
                         const Atlas::Objects::Entity::RootEntity & ent) const
{
}

EntityProperty * EntityProperty::copy() const
{
    return 0;
}


#endif /* STUBENTITYPROPERTY_H_ */
