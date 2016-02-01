/*
 Copyright (C) 2015 Piotr Stępnicki

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
#ifndef STUBMETABOLIZINGPROPERTY_H_
#define STUBMETABOLIZINGPROPERTY_H_


void MetabolizingProperty::grow(LocatedEntity *, float scale)
{
}

void MetabolizingProperty::scaleArea(LocatedEntity *)
{
}


void MetabolizingProperty::install(LocatedEntity * owner, const std::string & name)
{
}

void MetabolizingProperty::remove(LocatedEntity * owner, const std::string & name)
{
}

HandlerResult MetabolizingProperty::operation(LocatedEntity * e,
                                         const Operation & op,
                                         OpVector & res)
{
    return OPERATION_IGNORED;
}


MetabolizingProperty * MetabolizingProperty::copy() const
{
    return 0;
}




#endif /* STUBMETABOLIZINGPROPERTY_H_ */
