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

#include "rulesets/TerrainMod.h"

#include <cassert>

class TestInnerTerrainMod : public InnerTerrainMod
{
  public:
    TestInnerTerrainMod() : InnerTerrainMod("test") { }

    virtual bool parseAtlasData(Entity * owner, const Atlas::Message::MapType& modElement)
    {
        return true;
    }

    virtual Mercator::TerrainMod* getModifier()
    {
        return 0;
    }

};

int main()
{
    {
        InnerTerrainMod * titm = new TestInnerTerrainMod;
        delete titm;
    }
    {
        InnerTerrainMod * titm = new TestInnerTerrainMod;

        const std::string & type = titm->getTypename();
        assert(type == "test");

        delete titm;
    }
    return 0;
}

// stubs

#include "common/log.h"

void log(LogLevel lvl, const std::string & msg)
{
}
