// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseWorld.h"

#include "debug.h"

using Atlas::Message::Object;

static const bool debug_flag = false;

BaseWorld::BaseWorld(const std::string & id, Entity & gw) : OOGThing(id), m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}
