// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Stackable.h"
#include "StackFactory.h"

Stackable * StackFactory::newThing()
{
    return new Stackable();
}
