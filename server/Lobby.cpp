// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Lobby.h"
#include "Connection.h"

using Atlas::Message::Object;

oplist Lobby::operation(const RootOperation & op)
{
    const string & to = op.GetTo();
    if (to.empty() || to == "all") {
        adict_t::const_iterator I = accounts.begin();
        for (; I != accounts.end(); ++I) {
            Connection * c = I->second->connection;
            if (c != NULL) {
                cout << "Sending to " << I->first << " in lobby" << endl << flush;
                c->send(&op);
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

void Lobby::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    omap["name"] = "lobby";
    Object::ListType plist(1, "lobby");
    omap["parents"] = plist;
    Object::ListType player_list;
    adict_t::const_iterator I = accounts.begin();
    for(; I != accounts.end(); I++) {
        player_list.push_back(I->first);
    }
    omap["players"] = player_list;
}
