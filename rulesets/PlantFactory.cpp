// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Plant.h"
#include "PlantFactory.h"

Plant * PlantFactory::newThing()
{
    return new Plant();
}
