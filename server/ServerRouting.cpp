// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "ServerRouting.h"
#include "Lobby.h"

#include "common/BaseWorld.h"
#include "common/compose.hpp"
#include "common/const.h"
#include "common/debug.h"
#include "common/Monitors.h"
#include "common/Variable.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

BOOL_OPTION(restricted_flag, false, CYPHESIS, "restricted",
            "Flag to control restricted mode");

/// \brief Constructor for server object.
///
/// Requires a reference to the World management object, as well as the
/// ruleset and server name. Implicitly creates the Lobby management object.
ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name,
                             const std::string & id, long intId,
                             const std::string & lId, long lIntId) :
        Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*new Lobby(*this, lId, lIntId))
{
    Monitors * monitors = Monitors::instance();
    monitors->insert("server", "cyphesis");
    monitors->insert("builddate", String::compose("%1, %2",
                                                  consts::buildDate,
                                                  consts::buildTime));
    monitors->watch("instance", new Monitor<std::string>(::instance));
    monitors->watch("name", new Monitor<std::string>(m_svrName));
    monitors->watch("ruleset", new Monitor<std::string>(m_svrRuleset));
    monitors->watch("version", new Monitor<const char *>(consts::version));
    monitors->watch("buildid", new Monitor<int>(consts::buildId));
    monitors->watch("clients", new Monitor<int>(m_numClients));
}

/// Server destructor, implicitly destroys all OOG objects in the server.
ServerRouting::~ServerRouting()
{
    RouterMap::const_iterator Iend = m_objects.end();
    for(RouterMap::const_iterator I = m_objects.begin(); I != Iend; ++I) {
        delete I->second;
    }
    delete &m_lobby;
}

void ServerRouting::addToMessage(MapType & omap) const
{
    omap["objtype"] = "obj";
    omap["server"] = "cyphesis";
    omap["ruleset"] = m_svrRuleset;
    omap["name"] = m_svrName;
    omap["parents"] = ListType(1, "server");
    omap["clients"] = m_numClients;
    omap["uptime"] = m_world.upTime();
    omap["builddate"] = std::string(consts::buildTime)+", "+std::string(consts::buildDate);
    omap["buildid"] = consts::buildId;
    omap["version"] = std::string(consts::version);
    if (restricted_flag) {
        omap["restricted"] = "true";
    }
    omap["entities"] = (long)m_world.getEntities().size();
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}

void ServerRouting::addToEntity(const RootEntity & ent) const
{
    ent->setObjtype("obj");
    ent->setAttr("server", "cyphesis");
    ent->setAttr("ruleset", m_svrRuleset);
    ent->setName(m_svrName);
    ent->setParents(std::list<std::string>(1, "server"));
    ent->setAttr("clients", m_numClients);
    ent->setAttr("uptime", m_world.upTime());
    ent->setAttr("builddate", std::string(consts::buildTime)+", "+std::string(consts::buildDate));
    ent->setAttr("buildid", consts::buildId);
    ent->setAttr("version", std::string(consts::version));
    if (restricted_flag) {
        ent->setAttr("restricted", "true");
    }
    ent->setAttr("entities", (long)m_world.getEntities().size());
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}
