// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseWorld.h"

#include <rulesets/Entity.h>
#include <common/debug.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

BaseWorld::BaseWorld(Entity & gWorld) : gameWorld(gWorld)
{
}

BaseWorld::~BaseWorld()
{
    delete &gameWorld;
}
