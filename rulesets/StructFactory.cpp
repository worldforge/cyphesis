// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Structure.h"
#include "StructFactory.h"

StructureFactory * StructureFactory::m_instance = NULL;

Structure * StructureFactory::newThing()
{
    return new Structure();
}
