// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "OOGThing.h"

OOGThing::~OOGThing()
{
}

OpVector OOGThing::OtherOperation(const RootOperation & op) {
    return error(op, "Unknown operation");
}

