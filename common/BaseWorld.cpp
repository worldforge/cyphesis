// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#include "BaseWorld.h"

#include "log.h"
#include "debug.h"
#include "compose.hpp"

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
}

BaseWorld::~BaseWorld()
{
}

Entity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        log(ERROR, String::compose("Unable to convert ID \"%1\" to an integer", id).c_str());
    }

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        return I->second;
    } else {
        return 0;
    }
}
