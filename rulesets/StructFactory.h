// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef STRUCT_FACTORY_H
#define STRUCT_FACTORY_H

#include "ThingFactory.h"
#include "Structure.h"

class StructureFactory : public ThingFactory {
  public:
    StructureFactory() { }
    virtual Structure * newThing();
};

#endif // STRUCT_FACTORY_H
