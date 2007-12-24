// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: Persistor.cpp,v 1.40 2007-12-24 02:49:29 alriddoch Exp $

#error This file has been removed from the build.
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "Persistor.h"
#include "Persistor_impl.h"

#include "rulesets/Entity.h"
#include "rulesets/Thing.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/Structure.h"
#include "rulesets/World.h"

#include "common/Database.h"
#include "common/terrain_utils.h"

using Atlas::Message::MapType;

template<>
Persistor<Entity>::Persistor(bool temp) : m_class("entity")
{
    if (temp) { return; }
    MapType desc;
    desc["class"] = "                                                                                ";
    desc["type"] = "                                                                                ";
    // desc["loc"] = "                                                                                ";
    desc["loc"] = 0;
    desc["cont"] = 0;
    desc["px"] = 1.0;
    desc["py"] = 1.0;
    desc["pz"] = 1.0;
    desc["ox"] = 1.0;
    desc["oy"] = 1.0;
    desc["oz"] = 1.0;
    desc["ow"] = 1.0;

    desc["hasBox"] = 0xb001;
    desc["bnx"] = 1.0;
    desc["bny"] = 1.0;
    desc["bnz"] = 1.0;
    desc["bfx"] = 1.0;
    desc["bfy"] = 1.0;
    desc["bfz"] = 1.0;
    desc["status"] = 1.0;
    desc["name"] = "                                                                                ";
    desc["mass"] = 1.0;
    desc["seq"] = 0;
    desc["attributes"] = "";
    Database::instance()->registerEntityTable(m_class, desc);
}

template<>
Persistor<Thing>::Persistor(bool temp) : m_class("thing")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");
}

template<>
Persistor<Line>::Persistor(bool temp) : m_class("line")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<Area>::Persistor(bool temp) : m_class("area")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<Character>::Persistor(bool temp) : m_class("character")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    desc["sex"] = "        ";
    desc["drunkness"] = 1.0;
    desc["food"] = 1.0;
    desc["right_hand_wield"] = "        ";
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<Creator>::Persistor(bool temp) : m_class("creator")
{
    if (temp) { return; }
    // Creator entities are not persisted
}

template<>
Persistor<Plant>::Persistor(bool temp) : m_class("plant")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    desc["fruits"] = 1;
    desc["fruitName"] = "                                                                                ";
    desc["fruitChance"] = 1;
    desc["sizeAdult"] = 1.0;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<Stackable>::Persistor(bool temp) : m_class("stackable")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    desc["num"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<Structure>::Persistor(bool temp) : m_class("structure")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

template<>
Persistor<World>::Persistor(bool temp) : m_class("world")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");

    desc.clear();
    desc["height"] = 0.1f;
    Database::instance()->registerArrayTable("terrain", 2, desc);
}

template<>
void Persistor<Character>::update(Character * t)
{
    std::string columns;
    uCharacter(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

template<>
void Persistor<Creator>::update(Creator * t)
{
    // Creator entities are not persisted
    // Is this really needed? Probably not, as its never called.
}

template<>
void Persistor<Plant>::update(Plant * t)
{
    std::string columns;
    uPlant(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

template<>
void Persistor<World>::update(World * t)
{
    if (t->getUpdateFlags() & ~a_terrain) {
        std::string columns;
        uEntity(*t, columns);
        Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    }

    if (t->getUpdateFlags() & a_terrain) {
        updateStoredTerrain(t->getId(), t->terrain(), t->modifiedTerrain(),
                                                      t->createdTerrain());
        t->clearTerrainFlags();
    }

    t->clearUpdateFlags();
}

template<>
void Persistor<Character>::hookup(Character & t)
{
    t.updated.connect(sigc::bind(sigc::mem_fun(this,
                                               &Persistor<Character>::update),
                                 &t));
    t.destroyed.connect(sigc::bind(sigc::mem_fun(this,
                                                 &Persistor<Character>::remove),
                                   &t));
}

template<>
void Persistor<Character>::persist(Character & t)
{
    hookup(t);
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    cCharacter(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}

template<>
void Persistor<Plant>::persist(Plant & p)
{
    hookup(p);
    std::string columns;
    std::string values;
    cEntity(p, columns, values);
    cPlant(p, columns, values);
    Database::instance()->createEntityRow(m_class, p.getId(), columns, values);
}

template<>
void Persistor<Creator>::persist(Creator & t)
{
    // Creator entities are not persisted
}

template<>
void Persistor<World>::hookup(World & t)
{
    t.updated.connect(sigc::bind(sigc::mem_fun(this, &Persistor<World>::update),
                                 &t));
    // The game world should never be destroyed, so we don't connect
    // it to a remove function.
}

template<>
void Persistor<World>::cEntity(Entity & t, std::string & c, std::string & v)
{
    const char * cs = ", ";
    const char * sq = "'";
    if (!c.empty()) {
        c += cs;
    }
    c += "class, type, cont, px, py, pz, ox, oy, oz, ow, hasBox, bnx, bny, bnz, bfx, bfy, bfz, status, name, mass, seq, attributes";

    std::stringstream q;
    q << sq << m_class << sq << cs
      << sq << t.getType() << sq << cs
      << t.m_contains.size() << cs
      << t.m_location.pos().x() << cs
      << t.m_location.pos().y() << cs
      << t.m_location.pos().z() << cs;
    if (t.m_location.orientation().isValid()) {
        q << t.m_location.orientation().vector().x() << cs
          << t.m_location.orientation().vector().y() << cs
          << t.m_location.orientation().vector().z() << cs
          << t.m_location.orientation().scalar() << cs;
    } else {
        q << "0, 0, 0, 1, ";
    }
    if (t.m_location.bBox().isValid()) {
        q << "'t', "
          << t.m_location.bBox().lowCorner().x() << cs
          << t.m_location.bBox().lowCorner().y() << cs
          << t.m_location.bBox().lowCorner().z() << cs
          << t.m_location.bBox().highCorner().x() << cs
          << t.m_location.bBox().highCorner().y() << cs
          << t.m_location.bBox().highCorner().z() << cs;
    } else {
        q << "'f', 0, 0, 0, 0, 0, 0, ";
    }
    q << t.getSeq() << cs;

    if (t.getAttributes().empty()) {
        q << "''";
    } else {
        std::string aString;
        Database::instance()->encodeObject(t.getAttributes(), aString);
        q << "'" << aString << "'";
    }

    if (!v.empty()) {
        v += cs;
    }
    v += q.str();
}

template<>
void Persistor<World>::persist(World & t)
{
    hookup(t);
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);

    storeTerrain(t.getId(), t.terrain());
}

template class Persistor<Entity>;
template class Persistor<Thing>;
template class Persistor<Character>;
template class Persistor<Creator>;
template class Persistor<Plant>;
template class Persistor<Stackable>;
template class Persistor<Structure>;
template class Persistor<World>;
