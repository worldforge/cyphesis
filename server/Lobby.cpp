// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Lobby.h"

#include "Connection_methods.h"
#include "ServerRouting.h"

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

Lobby::Lobby(const std::string & id, ServerRouting & s) : OOGThing(id), server(s)
{
}

void Lobby::addObject(Account * ac)
{
    Appearance a(Appearance::Instantiate());
    Fragment::MapType us;
    us["id"] = ac->getId();
    us["loc"] = "lobby";
    a.SetArgs(Fragment::ListType(1,us));
    a.SetFrom(ac->getId());
    a.SetTo("lobby");
    a.SetSerialno(server.getSerialNo());
    operation(a);

    accounts[ac->getId()] = ac;
}

void Lobby::delObject(Account * a)
{
    Disappearance d(Disappearance::Instantiate());
    Fragment::MapType us;
    us["id"] = a->getId();
    us["loc"] = "lobby";
    d.SetArgs(Fragment::ListType(1,us));
    d.SetFrom(a->getId());
    d.SetTo("lobby");
    d.SetSerialno(server.getSerialNo());
    operation(d);

    accounts.erase(a->getId());
}


OpVector Lobby::operation(const RootOperation & op)
{
    const std::string & to = op.GetTo();
    if (to.empty() || to == "lobby") {
        AccountDict::const_iterator I = accounts.begin();
        RootOperation newop(op);
        for (; I != accounts.end(); ++I) {
            Connection * c = I->second->connection;
            if (c != NULL) {
                newop.SetTo(I->first);
                c->send(newop);
            }
        }
    } else {
        AccountDict::const_iterator I = accounts.find(to);
        if (I == accounts.end()) {
            return error(op, "Target account not logged in");
        } else {
            Connection * c = I->second->connection;
            if (c != NULL) {
                c->send(op);
            }
        }
    }
    return OpVector();
}

void Lobby::addToObject(Fragment::MapType & omap) const
{
    omap["objtype"] = "object";
    omap["id"] = "lobby";
    omap["name"] = "lobby";
    Fragment::ListType plist(1, "room");
    omap["parents"] = plist;
    Fragment::ListType player_list;
    AccountDict::const_iterator I = accounts.begin();
    for(; I != accounts.end(); I++) {
        player_list.push_back(I->first);
    }
    omap["people"] = player_list;
    omap["rooms"] = Fragment::ListType();
}
