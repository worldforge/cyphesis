// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#error This file has been removed from the build

#include "OOG_Thing.h"

OOGThing::~OOGThing()
{
}

oplist OOGThing::OtherOperation(const RootOperation & op) {
    return error(op, "Unknown operation");
}

