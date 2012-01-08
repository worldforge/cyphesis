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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "PropertyCoverage.h"

#include "rulesets/Entity.h"
#include "rulesets/TerrainModProperty.h"
#include "rulesets/TerrainProperty.h"

using Atlas::Message::ListType;
using Atlas::Message::MapType;

int main()
{
    {
        HandlerMap terrainModHandlers;

        TerrainModProperty * ap = new TerrainModProperty(terrainModHandlers);

        MapType shape;
        MapType mod;
        
        shape["type"] = "polygon";
        shape["points"] = ListType(3, ListType(2, 1.1f));
        mod["shape"] = shape;
        mod["type"] = "levelmod";

        ap->set(mod);

        // FIXME verify that the mod really takes effect
        // ap->apply(0);
    }

}

// stubs

#include "common/log.h"

void log(LogLevel lvl, const std::string & msg)
{
}
