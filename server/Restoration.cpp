// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Restoration.h"

#include <common/Database.h>

#include <iostream>

Restoration::Restoration(ServerRouting & svr) : server(svr),
                                                database(*Database::instance())
{
}

void Restoration::restore(const std::string & id, const std::string & classn)
{
    database.selectClassByLoc(id);
}

void Restoration::read()
{
    DatabaseResult res = database.selectClassByLoc("");
    if (res.error()) {
        std::cout << "DEBUG: Problem getting root id from world db"
                  << std::endl << std::flush;
        return;
    }
    if (res.empty()) {
        std::cout << "DEBUG: No world in database"
                  << std::endl << std::flush;
        database.clearTable("entity_ent");
        return;
    }
    if (res.size() > 1) {
        std::cout << "DEBUG: More than one root entity in database"
                  << std::endl << std::flush;
        database.clearTable("entity_ent");
        return;
    }
    std::string rootId = res.field(0, "id");
    std::string rootClass = res.field(0, "class");
    if (rootId.empty() || rootClass.empty()) {
        std::cout << "DEBUG: Stuff empty"
                  << std::endl << std::flush;
        return;
    }

    
    // getRootFromTable();
    // handleRootAttrs();
    // getListChildren();
    // getChildrenCorrectTable();
    // reconstruct();
    // recurse();
}
