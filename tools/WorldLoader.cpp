// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include "WorldLoader.h"

#include "tools/MultiLineListFormatter.h"

#include "common/AtlasFileLoader.h"
#include "common/compose.hpp"
#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Get;

void WorldLoader::infoArrived(const Operation & op, OpVector & res)
{
}

WorldLoader::WorldLoader(const std::string & accountId) : m_account(accountId),
                                                          m_lastSerialNo(-1),
                                                          m_count(0)
{
}

WorldLoader::~WorldLoader()
{
}

void WorldLoader::setup(const std::string & arg, OpVector & res)
{
    std::string filename("world.xml");
    if (!arg.empty()) {
        filename = arg;
    }

    AtlasFileLoader loader(filename, m_objects);

    if (!loader.isOpen()) {
        log(ERROR, String::compose("Unable to open %1", filename));
        m_complete = true;
        return;
    }
    loader.read();
    std::cout << "LOADED " << m_objects.size() << std::endl << std::flush;
    // Send initiating op.
        m_complete = true;
}

void WorldLoader::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO) {
        infoArrived(op, res);
    }
}
