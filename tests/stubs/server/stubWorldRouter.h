/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef STUBWORLDROUTER_H_
#define STUBWORLDROUTER_H_


WorldRouter::WorldRouter(const SystemTime &) :
      BaseWorld(*new Entity(consts::rootWorldId, consts::rootWorldIntId)),
      m_entityCount(1)

{
}

WorldRouter::~WorldRouter()
{
}

LocatedEntity * WorldRouter::addEntity(LocatedEntity * ent)
{
    return 0;
}

LocatedEntity * WorldRouter::addNewEntity(const std::string & typestr,
                                          const Atlas::Objects::Entity::RootEntity & attrs)
{
    return 0;
}

void WorldRouter::delEntity(LocatedEntity * obj)
{
}

int WorldRouter::createSpawnPoint(const Atlas::Message::MapType & data, LocatedEntity * ent)
{
    return 0;
}

int WorldRouter::removeSpawnPoint(LocatedEntity *)
{
    return 0;
}

int WorldRouter::getSpawnList(Atlas::Message::ListType & data)
{
    return 0;
}

LocatedEntity * WorldRouter::spawnNewEntity(const std::string & name,
                                            const std::string & type,
                                            const Atlas::Objects::Entity::RootEntity & desc)
{
    return 0;
}

int WorldRouter::moveToSpawn(const std::string & name,
                        Location& location)
{
    return 0;
}


Task * WorldRouter::newTask(const std::string & name, LocatedEntity & owner)
{
    return 0;
}

Task * WorldRouter::activateTask(const std::string & tool,
                                 const std::string & op,
                                 LocatedEntity * target,
                                 LocatedEntity & owner)
{
    return 0;
}

void WorldRouter::message(const Operation & op, LocatedEntity & ent)
{
}

bool WorldRouter::idle()
{
    return false;
}

LocatedEntity * WorldRouter::findByName(const std::string & name)
{
    return 0;
}

LocatedEntity * WorldRouter::findByType(const std::string & type)
{
    return 0;
}

ArithmeticScript * WorldRouter::newArithmetic(const std::string & name,
                                              LocatedEntity * owner)
{
    return 0;
}

void WorldRouter::addPerceptive(LocatedEntity * perceptive)
{
}

void WorldRouter::resumeWorld()
{
}


#endif /* STUBWORLDROUTER_H_ */
