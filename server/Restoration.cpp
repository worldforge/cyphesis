// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Restoration.h"
#include "Restorer.h"
#include "Persistor.h"
#include "ServerRouting.h"

#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Food.h"
#include "rulesets/Area.h"
#include "rulesets/Line.h"
#include "rulesets/Structure.h"
#include "rulesets/Stackable.h"
#include "rulesets/World.h"

#include "common/const.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/Database.h"
#include "common/BaseWorld.h"

#include <iostream>

static const bool debug_flag = false;

Restoration::Restoration(ServerRouting & svr) : server(svr),
                                                database(*Database::instance())
{
    m_restorers["entity"] = &Restorer<Entity>::restore;
    m_restorers["thing"] = &Restorer<Thing>::restore;
    m_restorers["character"] = &Restorer<Character>::restore;
    m_restorers["creator"] = &Restorer<Creator>::restore;
    m_restorers["plant"] = &Restorer<Plant>::restore;
    m_restorers["food"] = &Restorer<Food>::restore;
    m_restorers["line"] = &Restorer<Line>::restore;
    m_restorers["area"] = &Restorer<Area>::restore;
    m_restorers["structure"] = &Restorer<Structure>::restore;
    m_restorers["stackable"] = &Restorer<Stackable>::restore;
}

void Restoration::restoreChildren(Entity * loc)
{
    const std::string & parent = loc->getId();
    DatabaseResult res = database.selectClassByLoc(parent);
    if (res.error()) {
        debug(std::cout << "DEBUG: Problem getting " << parent
                        << "'s child list from world db"
                        << std::endl << std::flush;);
        return;
    }
    if (res.empty()) {
        res.clear();
        debug(std::cout << "DEBUG: No " << parent << " children in database"
                        << std::endl << std::flush;);
        return;
    }
    std::set<std::string> classes;
    DatabaseResult::const_iterator Iend = res.end();
    for (DatabaseResult::const_iterator I = res.begin(); I != Iend; ++I) {
        // As we are iterating, we should find the number of the
        // column first, and then use that. Cheaper.
        std::string child_id = I.column("id");
        std::string child_class = I.column("class");
        if (child_id.empty() || child_class.empty()) {
            log(ERROR, "Malformed (id,class) record from database.");
            debug(std::cout << "DEBUG: Empty record when reading children of "
                            << parent << std::endl << std::flush;);
            continue;
        }
        classes.insert(child_class);
        debug(std::cout << "DEBUG: Child is " << child_id
                        << " with class " << child_class
                        << std::endl << std::flush;);
    }
    res.clear();
    std::list<Entity *> children;
    std::set<std::string>::const_iterator J = classes.begin();
    std::set<std::string>::const_iterator Jend = classes.end();
    for(; J != Jend; ++J) {
        RestoreDict::const_iterator K = m_restorers.find(*J);
        if (K == m_restorers.end()) {
            log(ERROR, "Could not find a restorer for class");
            continue;
        }
        restoreFunc restorer = K->second;
        res = database.selectOnlyByLoc(parent, *J);
        if (res.error()) {
            log(ERROR, "Database query error.");
            debug(std::cout << "DEBUG: Problem getting " << parent
                            << "'s child list from world db"
                            << std::endl << std::flush;);
            return;
        }
        if (res.empty()) {
            res.clear();
            debug(std::cout << "DEBUG: No " << parent << " children in database"
                            << std::endl << std::flush;);
            continue;
        }
        DatabaseResult::const_iterator Lend = res.end();
        for (DatabaseResult::const_iterator L = res.begin(); L != Lend; ++L) {
            const char * id = L.column("id");
            if (id == 0) {
                continue;
            }
            Entity * ent = restorer(id, L);
            ent->m_location.m_loc = loc;
            server.m_world.addObject(ent, true);
            const char * c = L.column("cont");
            if (c != 0) {
                if (*c != '0') {
                    children.push_back(ent);
                }
            }
        }
        res.clear();
    }
    std::list<Entity *>::const_iterator L = children.begin();
    std::list<Entity *>::const_iterator Lend = children.end();
    for (; L != Lend; ++L) {
        restoreChildren(*L);
    }
}

/// Read and restore the world state from the database.
///
/// @returns -1 if an error occurs, 1 if no world state was present in
/// the database or 0 of world state was restored normally.
int Restoration::read()
{
    DatabaseResult res = database.selectOnlyByLoc("", "world");
    if (res.error()) {
        log(ERROR, "Database error retrieving root world.");
        debug(std::cout << "DEBUG: Problem getting root id from world db"
                        << std::endl << std::flush;);
        return -1;
    }
    if (res.empty()) {
        res.clear();
        debug(std::cout << "DEBUG: No world in database"
                        << std::endl << std::flush;);
        database.clearTable("entity_ent");
        return 1;
    }
    if (res.size() > 1) {
        res.clear();
        debug(std::cout << "DEBUG: More than one root entity in database"
                        << std::endl << std::flush;);
        database.clearTable("entity_ent");
        return -1;
    }
    std::string rootId = res.field("id");
    std::string rootClass = res.field("class");
    if (rootId.empty() || rootClass.empty()) {
        res.clear();
        debug(std::cout << "DEBUG: Stuff empty" << std::endl << std::flush;);
        return -1;
    }
    debug(std::cout << "DEBUG: World is " << rootId << " with class "
                    << rootClass << std::endl << std::flush;);

    // FIXME - restore attributes of the gameWorld object itself.
    DatabaseResult::const_iterator I = res.begin();
    Restorer<World> & world = (Restorer<World> &)server.m_world.m_gameWorld;
    world.populate(I);
    if (consts::enable_persistence) {
        Restorer<World>::m_persist.hookup(world);
    }
    res.clear();
    restoreChildren(&server.m_world.m_gameWorld);
    return 0;
}
