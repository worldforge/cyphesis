// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ServerRouting_methods.h"
#include "Persistance.h"
#include "CommServer.h"
#include "Account.h"
#include "Lobby.h"

#include <common/debug.h>

#include <iostream>

static bool debug_flag = false;

using Atlas::Message::Object;

ServerRouting::ServerRouting(CommServer & server, const std::string & ruleset,
                             const std::string & name) :
        commServer(server), svrRuleset(ruleset), svrName(name),
        world(*new WorldRouter(*this)), lobby(*new Lobby(*this))
{
    setId(name);
    Account * adm = Persistance::loadAdminAccount();
    addObject(adm);
    adm->world=&world;
}

ServerRouting::~ServerRouting()
{
    BaseDict::const_iterator I = objects.begin();
    for(; I != objects.end(); I++) {
        debug(std::cout << "Del " << I->second->getId() << std::endl
                        << std::flush;);
        delete I->second;
    }
    delete &world;
    delete &lobby;
}

void ServerRouting::addToObject(Object::MapType & omap) const
{
    omap["server"] = "cyphesis";
    omap["ruleset"] = svrRuleset;
    omap["name"] = svrName;
    Object::ListType plist(1, "server");
    omap["parents"] = plist;
    omap["clients"] = commServer.numClients();
    omap["uptime"] = world.upTime();
    omap["builddate"] = std::string(__TIME__) + ", " + std::string(__DATE__);
    omap["version"] = std::string(VERSION);
    if (Persistance::restricted) {
        omap["restricted"] = "true";
    }
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}
