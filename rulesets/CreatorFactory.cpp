// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Creator.h"
#include "CreatorFactory.h"

CreatorFactory * CreatorFactory::m_instance = NULL;

Creator * CreatorFactory::newThing()
{
    return new Creator();
}
