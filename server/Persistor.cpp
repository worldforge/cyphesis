// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Persistor.h"
#include "Persistor_impl.h"

#include "rulesets/Entity.h"
#include "rulesets/Thing.h"
#include "rulesets/Line.h"
#include "rulesets/Area.h"
#include "rulesets/Character.h"
#include "rulesets/Creator.h"
#include "rulesets/Food.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/Structure.h"
#include "rulesets/World.h"

#include "common/Database.h"
#include "common/terrain_utils.h"

template class Persistor<Entity>;
template class Persistor<Thing>;
template class Persistor<Line>;
template class Persistor<Area>;
template class Persistor<Character>;
template class Persistor<Creator>;
template class Persistor<Food>;
template class Persistor<Plant>;
template class Persistor<Stackable>;
template class Persistor<Structure>;
template class Persistor<World>;

using Atlas::Message::MapType;

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

Persistor<Thing>::Persistor<Thing>(bool temp) : m_class("thing")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");
}

Persistor<Line>::Persistor<Line>(bool temp) : m_class("line")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Area>::Persistor<Area>(bool temp) : m_class("area")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Character>::Persistor<Character>(bool temp) : m_class("character")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    desc["sex"] = "        ";
    desc["drunkness"] = 1.0;
    desc["food"] = 1.0;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Creator>::Persistor<Creator>(bool temp) : m_class("creator")
{
    if (temp) { return; }
    // Creator entities are not persisted
}

Persistor<Plant>::Persistor<Plant>(bool temp) : m_class("plant")
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

Persistor<Food>::Persistor<Food>(bool temp) : m_class("food")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Stackable>::Persistor<Stackable>(bool temp) : m_class("stackable")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    desc["num"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Structure>::Persistor<Structure>(bool temp) : m_class("structure")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<World>::Persistor<World>(bool temp) : m_class("world")
{
    if (temp) { return; }
    MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");

    desc.clear();
    desc["height"] = 0.1f;
    Database::instance()->registerArrayTable("terrain", 2, desc);
}

void Persistor<Character>::update(Character * t)
{
    std::string columns;
    uCharacter(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

void Persistor<Creator>::update(Creator * t)
{
    // Creator entities are not persisted
    // Is this really needed? Probably not, as its never called.
}

void Persistor<Line>::update(Line * t)
{
    std::string columns;
    uLine(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

void Persistor<Area>::update(Area * t)
{
    std::string columns;
    uArea(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

void Persistor<Plant>::update(Plant * t)
{
    std::string columns;
    uPlant(*t, columns);
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

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

void Persistor<Character>::hookup(Character & t)
{
    t.updated.connect(SigC::bind<Character *>(SigC::slot(*this,
                                              &Persistor<Character>::update),
                                              &t));
    t.destroyed.connect(SigC::bind<Character *>(SigC::slot(*this,
                                                &Persistor<Character>::remove),
                                                &t));
}

void Persistor<Character>::persist(Character & t)
{
    hookup(t);
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    cCharacter(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}

void Persistor<Plant>::persist(Plant & p)
{
    hookup(p);
    std::string columns;
    std::string values;
    cEntity(p, columns, values);
    cPlant(p, columns, values);
    Database::instance()->createEntityRow(m_class, p.getId(), columns, values);
}

void Persistor<Creator>::persist(Creator & t)
{
    // Creator entities are not persisted
}

void Persistor<World>::hookup(World & t)
{
    t.updated.connect(SigC::bind<World *>(SigC::slot(*this,
                                          &Persistor<World>::update), &t));
    // The game world should never be destroyed, so we don't connect
    // it to a remove function.
}

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
      << t.m_location.m_pos.x() << cs
      << t.m_location.m_pos.y() << cs
      << t.m_location.m_pos.z() << cs;
    if (t.m_location.m_orientation.isValid()) {
        q << t.m_location.m_orientation.vector().x() << cs
          << t.m_location.m_orientation.vector().y() << cs
          << t.m_location.m_orientation.vector().z() << cs
          << t.m_location.m_orientation.scalar() << cs;
    } else {
        q << "0, 0, 0, 1, ";
    }
    if (t.m_location.m_bBox.isValid()) {
        q << "'t', "
          << t.m_location.m_bBox.lowCorner().x() << cs
          << t.m_location.m_bBox.lowCorner().y() << cs
          << t.m_location.m_bBox.lowCorner().z() << cs
          << t.m_location.m_bBox.highCorner().x() << cs
          << t.m_location.m_bBox.highCorner().y() << cs
          << t.m_location.m_bBox.highCorner().z() << cs;
    } else {
        q << "'f', 0, 0, 0, 0, 0, 0, ";
    }
    q << t.getStatus() << cs
      << sq << t.getName() << sq << cs
      << t.getMass() << cs
      << t.getSeq() << cs;

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

void Persistor<World>::persist(World & t)
{
    hookup(t);
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);

    storeTerrain(t.getId(), t.terrain());
}
