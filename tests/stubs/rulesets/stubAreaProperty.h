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
#ifndef STUBAREAPROPERTY_H_
#define STUBAREAPROPERTY_H_


AreaProperty::AreaProperty()
{
}

AreaProperty::~AreaProperty()
{
}

void AreaProperty::set(const Atlas::Message::Element & ent)
{
}

AreaProperty * AreaProperty::copy() const
{
    return 0;
}

void AreaProperty::apply(LocatedEntity * owner)
{
}


#endif /* STUBAREAPROPERTY_H_ */
