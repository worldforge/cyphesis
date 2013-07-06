// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/Spawn.h"

#include <cassert>

class TestSpawn : public Spawn
{
  public:
    TestSpawn()  { }

    virtual ~TestSpawn()
    {
    }

    virtual int spawnEntity(const std::string & type,
                            const Atlas::Objects::Entity::RootEntity & dsc)
    {
        return 0;
    }

    virtual int populateEntity(LocatedEntity * ent,
                               const Atlas::Objects::Entity::RootEntity & dsc,
                               OpVector & res)
    {
        return 0;
    }

    virtual int addToMessage(Atlas::Message::MapType & msg) const
    {
        return 0;
    }
};

int main()
{
    {
        Spawn * s = new TestSpawn;

        delete s;
    }
    return 0;
}
