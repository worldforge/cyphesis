// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Lobby.h"

#include "Connection_methods.h"
#include "ServerRouting.h"

#include "common/debug.h"
#include "common/serialno.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Disappearance;
using Atlas::Objects::Entity::Anonymous;

static const bool debug_flag = false;

Lobby::Lobby(ServerRouting & s, const std::string & id, long intId) :
             OOGThing(id, intId), m_server(s)
{
}

Lobby::~Lobby()
{
}

void Lobby::addAccount(Account * ac)
{
    debug(std::cout << "Lobby::addAccount(" << ac->getId() << ")"
                    << std::endl << std::flush;);

    Appearance a;
    Anonymous us;
    us->setId(ac->getId());
    us->setLoc(getId());
    a->setArgs1(us);
    a->setFrom(ac->getId());
    a->setTo(getId());
    a->setSerialno(newSerialNo());

    OpVector res;
    operation(a, res);
    assert(res.empty());

    m_accounts[ac->getId()] = ac;
}

void Lobby::delAccount(Account * ac)
{
    debug(std::cout << "Lobby::delAccount(" << ac->getId() << ")"
                    << std::endl << std::flush;);
                    
    Disappearance d;
    Anonymous us;
    us->setId(ac->getId());
    us->setLoc(getId());
    d->setArgs1(us);
    d->setFrom(ac->getId());
    d->setTo(getId());
    d->setSerialno(newSerialNo());

    OpVector res;
    operation(d, res);
    assert(res.empty());

    m_accounts.erase(ac->getId());
}


void Lobby::operation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Lobby::operation(" << op->getParents().front()
                                           << std::endl << std::flush; );
    const std::string & to = op->getTo();
    if (to.empty() || to == getId()) {
        Operation newop(op.copy());
        AccountDict::const_iterator I = m_accounts.begin();
        AccountDict::const_iterator Iend = m_accounts.end();
        for (; I != Iend; ++I) {
            Connection * c = I->second->m_connection;
            if (c != 0) {
                newop->setTo(I->first);
                debug(std::cout << "Lobby sending " << newop->getParents().front() << " operation to " << I->first << std::endl << std::flush; );
                c->send(newop);
            }
        }
    } else {
        AccountDict::const_iterator I = m_accounts.find(to);
        if (I == m_accounts.end()) {
            error(op, "Target account not logged in", res);
        } else {
            Connection * c = I->second->m_connection;
            if (c == 0) {
                error(op, "Target account not logged in", res);
            } else {
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

void Lobby::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
    ent->setName("lobby");
    ListType plist(1, "room");
    ent->setParents(std::list<std::string>(1,"room"));
    ListType player_list;
    AccountDict::const_iterator Iend = m_accounts.end();
    for (AccountDict::const_iterator I = m_accounts.begin(); I != Iend; ++I) {
        player_list.push_back(I->first);
    }
    ent->setAttr("people", player_list);
    ent->setAttr("rooms", ListType());
    BaseEntity::addToEntity(ent);
}
