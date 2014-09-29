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
#ifndef STUBLIMBOPROPERTY_H_
#define STUBLIMBOPROPERTY_H_


LimboProperty::LimboProperty()
{
}

LimboProperty * LimboProperty::copy() const
{
    return nullptr;
}

void LimboProperty::install(LocatedEntity * ent, const std::string & name)
{
}

void LimboProperty::remove(LocatedEntity * ent, const std::string & name)
{
}


#endif /* STUBLIMBOPROPERTY_H_ */
