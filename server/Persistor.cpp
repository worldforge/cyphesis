// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Persistor.h"

#include <common/Database.h>

class Entity;
class Thing;
class Line;
class Area;
class Character;
class Creator;
class Food;
class Plant;
class Stackable;
class Structure;

Persistor<Entity>::Persistor<Entity>() : m_class("entity")
{
    Atlas::Message::Object::MapType desc;
    desc["name"] = "                                                                                ";
    desc["mass"] = 1.0;
    desc["status"] = 1.0;
    desc["seq"] = 0;
    Database::instance()->registerEntityTable("entity", desc);
}

Persistor<Thing>::Persistor<Thing>() : m_class("thing")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("thing", desc, "entity");
}

Persistor<Line>::Persistor<Line>() : m_class("line")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("line", desc, "thing");
}

Persistor<Area>::Persistor<Area>() : m_class("area")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("area", desc, "thing");
}

Persistor<Character>::Persistor<Character>() : m_class("character")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["sex"] = "        ";
    desc["drunkness"] = 1.0;
    desc["food"] = 1.0;
    Database::instance()->registerEntityTable("character", desc, "thing");
}

Persistor<Creator>::Persistor<Creator>() : m_class("creator")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("creator", desc, "character");
}

Persistor<Plant>::Persistor<Plant>() : m_class("plant")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["fruits"] = 1;
    Database::instance()->registerEntityTable("plant", desc, "thing");
}

Persistor<Food>::Persistor<Food>() : m_class("food")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("food", desc, "thing");
}

Persistor<Stackable>::Persistor<Stackable>() : m_class("stackable")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["num"] = 1;
    Database::instance()->registerEntityTable("stackable", desc, "thing");
}

Persistor<Structure>::Persistor<Structure>() : m_class("structure")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable("structure", desc, "thing");
}
