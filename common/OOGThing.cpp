// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "OOGThing.h"

/// \brief Constructor called from classes that inherit from OOGThing
///
/// @param id identifier of new entity
OOGThing::OOGThing(const std::string & id, long intId) : BaseEntity(id, intId)
{
}

OOGThing::~OOGThing()
{
}

void OOGThing::OtherOperation(const Operation & op, OpVector & res)
{
    
    error(op, "Unknown operation", res);
}

