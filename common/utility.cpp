// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "utility.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

namespace utility {

bool Object_asOperation(const MapType & ent, RootOperation & op)
{
    Root r = Atlas::Objects::Factories::instance()->createObject(ent);
    op = Atlas::Objects::smart_dynamic_cast<RootOperation>(r);
    return op.isValid();
}

} // namespace utility
