// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Lobby.h"

#include "Connection_methods.h"
#include "ServerRouting.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;

static const bool debug_flag = false;

Lobby::Lobby(const std::string & id, ServerRouting & s) : OOGThing(id), m_server(s)
{
}

void Lobby::addObject(Account * ac)
{
    debug(std::cout << "Lobby::addObject(" << ac->getId() << ")"
                    << std::endl << std::flush;);

    Appearance a;
    ListType & args = a.getArgs();
    args.push_back(MapType());
    MapType & us = args.back().asMap();
    us["id"] = ac->getId();
    us["loc"] = "lobby";
    a.setFrom(ac->getId());
    a.setTo("lobby");
    a.setSerialno(m_server.newSerialNo());

    OpVector res;
    operation(a, res);
    assert(res.empty());

    m_accounts[ac->getId()] = ac;
}

void Lobby::delObject(Account * a)
{
    debug(std::cout << "Lobby::delObject(" << a->getId() << ")"
                    << std::endl << std::flush;);
                    
    Disappearance d;
    ListType & args = d.getArgs();
    args.push_back(MapType());
    MapType & us = args.back().asMap();
    us["id"] = a->getId();
    us["loc"] = "lobby";
    d.setFrom(a->getId());
    d.setTo("lobby");
    d.setSerialno(m_server.newSerialNo());

    OpVector res;
    operation(d, res);
    assert(res.empty());

    m_accounts.erase(a->getId());
}


void Lobby::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Lobby::operation(" << op.getParents().front().asString()
                                           << std::endl << std::flush; );
    const std::string & to = op.getTo();
    if (to.empty() || to == "lobby") {
        Operation newop(op);
        AccountDict::const_iterator I = m_accounts.begin();
        AccountDict::const_iterator Iend = m_accounts.end();
        for (; I != Iend; ++I) {
            Connection * c = I->second->m_connection;
            if (c != NULL) {
                newop.setTo(I->first);
                debug(std::cout << "Lobby sending " << newop.getParents().front().asString() << " operation to " << I->first << std::endl << std::flush; );
                c->send(newop);
            }
        }
    } else {
        AccountDict::const_iterator I = m_accounts.find(to);
        if (I == m_accounts.end()) {
            error(op, "Target account not logged in", res);
            return;
        } else {
            Connection * c = I->second->m_connection;
            if (c != NULL) {
                c->send(op);
            }
        }
    }
}

void Lobby::addToMessage(MapType & omap) const
{
    omap["name"] = "lobby";
    ListType plist(1, "room");
    omap["parents"] = plist;
    ListType player_list;
    AccountDict::const_iterator Iend = m_accounts.end();
    for (AccountDict::const_iterator I = m_accounts.begin(); I != Iend; ++I) {
        player_list.push_back(I->first);
    }
    omap["people"] = player_list;
    omap["rooms"] = ListType();
    BaseEntity::addToMessage(omap);
}
