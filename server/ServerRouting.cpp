// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ServerRouting.h"
#include "Lobby.h"

#include "common/debug.h"
#include "common/const.h"
#include "common/BaseWorld.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/RootEntity.h>

#include <iostream>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Entity::RootEntity;

static bool debug_flag = false;

/// \brief Constructor for server object.
///
/// Requires a reference to the World management object, as well as the
/// ruleset and server name. Implicitly creates the Lobby management object.
ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name) :
        OOGThing(name, -1), m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*new Lobby(*this, "lobby", -1))
{
}

/// Server destructor, implicitly destroys all OOG objects in the server.
ServerRouting::~ServerRouting()
{
    BaseDict::const_iterator Iend = m_objects.end();
    for(BaseDict::const_iterator I = m_objects.begin(); I != Iend; ++I) {
        debug(std::cout << "Del " << I->second->getId() << std::endl
                        << std::flush;);
        delete I->second;
    }
    delete &m_lobby;
}

/// Copies a representation of the server into an Atlas message.
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
    omap["version"] = std::string(consts::version);
    if (restricted_flag) {
        omap["restricted"] = "true";
    }
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}

/// Copies a representation of the server into an Atlas entity.
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
    ent->setAttr("version", std::string(consts::version));
    if (restricted_flag) {
        ent->setAttr("restricted", "true");
    }
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}
