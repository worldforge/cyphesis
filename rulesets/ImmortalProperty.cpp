/*
 Copyright (C) 2013 Erik Ogenvik

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

#include "ImmortalProperty.h"
#include "LocatedEntity.h"

#include <Atlas/Objects/Operation.h>

static const bool debug_flag = false;

ImmortalProperty::ImmortalProperty()
{
}

ImmortalProperty::~ImmortalProperty()
{
}

void ImmortalProperty::install(LocatedEntity * owner, const std::string & name)
{
    owner->installDelegate(Atlas::Objects::Operation::DELETE_NO, name);
}

void ImmortalProperty::apply(LocatedEntity * ent)
{

}

HandlerResult ImmortalProperty::operation(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    return delete_handler(e, op, res);
}

ImmortalProperty * ImmortalProperty::copy() const
{
    return new ImmortalProperty(*this);
}

HandlerResult ImmortalProperty::delete_handler(LocatedEntity * e,
        const Operation & op, OpVector & res)
{
    if (m_data == 1) {
        //This will effectively make the system ignore the Delete op.
        return OPERATION_BLOCKED;
    }
    return OPERATION_IGNORED;
}



