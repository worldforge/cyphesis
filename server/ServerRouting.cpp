// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "ServerRouting.h"
#include "WorldRouter.h"
#include "Lobby.h"

#include <common/debug.h>
#include <common/const.h>

#include <iostream>

static bool debug_flag = false;

ServerRouting::ServerRouting(const std::string & ruleset, const std::string & name) :
        OOGThing(name), svrRuleset(ruleset), svrName(name), numClients(0),
        world(*new WorldRouter(*this)), lobby(*new Lobby("lobby", *this))
{
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

void ServerRouting::addToObject(Fragment::MapType & omap) const
{
    omap["server"] = "cyphesis";
    omap["ruleset"] = svrRuleset;
    omap["name"] = svrName;
    omap["parents"] = Fragment::ListType(1, "server");
    omap["clients"] = numClients;
    omap["uptime"] = world.upTime();
    omap["builddate"] = std::string(consts::buildTime)+", "+std::string(consts::buildDate);
    omap["version"] = std::string(consts::version);
    if (restricted_flag) {
        omap["restricted"] = "true";
    }
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}

#if defined(__GNUC__) && __GNUC__ < 3 && __GNUC_MINOR__ < 96

int ServerRouting::idle() {
    return world.idle();
}

#endif // defined(__GNUC__) && __GNUC_MINOR__ <= 96
