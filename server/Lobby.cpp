// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Lobby.h"
#include "Connection.h"

#include <iostream>

using Atlas::Message::Object;

oplist Lobby::operation(const RootOperation & op)
{
    const std::string & to = op.GetTo();
    if (to.empty() || to == "lobby") {
        adict_t::const_iterator I = accounts.begin();
        RootOperation newop(op);
        for (; I != accounts.end(); ++I) {
            Connection * c = I->second->connection;
            if (c != NULL) {
                std::cout << "Sending to " << I->first << " in lobby"
                          << std::endl << std::flush;
                newop.SetTo(I->first);
                c->send(&newop);
            }
        }
    } else {
        adict_t::const_iterator I = accounts.find(to);
        if (I == accounts.end()) {
            return error(op, "Target account not logged in");
        } else {
            Connection * c = I->second->connection;
            if (c != NULL) {
                c->send(&op);
            }
        }
    }
    return oplist();
}

void Lobby::addToObject(Object::MapType & omap) const
{
    omap["id"] = "lobby";
    omap["name"] = "lobby";
    Object::ListType plist(1, "room");
    omap["parents"] = plist;
    Object::ListType player_list;
    adict_t::const_iterator I = accounts.begin();
    for(; I != accounts.end(); I++) {
        player_list.push_back(I->first);
    }
    omap["people"] = player_list;
    omap["rooms"] = Object::ListType();
}
