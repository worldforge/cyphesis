// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Stackable.h"
#include "StackFactory.h"

StackFactory * StackFactory::m_instance = NULL;

Stackable * StackFactory::newThing()
{
    return new Stackable();
}
