// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ServerRouting.h"
#include "Lobby.h"

#include "common/debug.h"
#include "common/const.h"
#include "common/BaseWorld.h"

#include <Atlas/Message/Element.h>

#include <iostream>

static bool debug_flag = false;

/// \brief Constructor for server object.
///
/// Requires a reference to the World management object, as well as the
/// ruleset and server name. Implicitly creates the Lobby management object.
ServerRouting::ServerRouting(BaseWorld & wrld,
                             const std::string & ruleset,
                             const std::string & name) :
        OOGThing(name), m_svrRuleset(ruleset), m_svrName(name), m_numClients(0),
        m_world(wrld), m_lobby(*new Lobby("lobby", *this))
{
}

/// Server destructor, implicitly destroys all OOG objects in the server.
ServerRouting::~ServerRouting()
{
    BaseDict::const_iterator I = m_objects.begin();
    for(; I != m_objects.end(); I++) {
        debug(std::cout << "Del " << I->second->getId() << std::endl
                        << std::flush;);
        delete I->second;
    }
    delete &m_lobby;
}

/// Copies a representation of the server into an Atlas message.
void ServerRouting::addToObject(MapType & omap) const
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

#if defined(__GNUC__) && __GNUC__ < 3 && __GNUC_MINOR__ < 96

bool ServerRouting::idle() {
    return world.idle();
}

#endif // defined(__GNUC__) && __GNUC_MINOR__ <= 96
