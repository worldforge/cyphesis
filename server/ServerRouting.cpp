// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "ServerRouting_methods.h"
#include "CommServer.h"
#include "Account.h"
#include "Lobby.h"

#include <common/persistance.h>

using Atlas::Message::Object;

ServerRouting::ServerRouting(CommServer & server, const string & name) :
        commServer(server), svrName(name), world(*new WorldRouter(*this)),
        lobby(*new Lobby())
{
    fullid = name;
    idDict[fullid] = this;
    Account * adm = Persistance::load_admin_account();
    addObject(adm);
    adm->world=&world;
}

ServerRouting::~ServerRouting()
{
    idDict.erase(fullid);
    idDict.erase(world.fullid);
    dict_t::const_iterator I = idDict.begin();
    for(; I != idDict.end(); I++) {
        cout << "Del " << I->second->fullid << endl << flush;
        delete I->second;
    }
    delete &world;
    delete &lobby;
}

void ServerRouting::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    omap["server"] = "cyphesis";
    omap["ruleset"] = svrName;
    Object::ListType plist(1, "server");
    omap["parents"] = plist;
    omap["clients"] = commServer.numClients();
    omap["uptime"] = world.upTime();
    if (Persistance::restricted) {
        omap["restricted"] = "true";
    }
    
    // We could add all sorts of stats here, but I don't know exactly what yet.
}
