// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Creator.h"
#include "CreatorFactory.h"

Creator * CreatorFactory::newThing()
{
    return new Creator();
}
