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

Persistor<Entity>::Persistor(bool temp) : m_class("entity")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
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
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");
}

Persistor<Line>::Persistor<Line>(bool temp) : m_class("line")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Area>::Persistor<Area>(bool temp) : m_class("area")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Character>::Persistor<Character>(bool temp) : m_class("character")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
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
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    desc["fruits"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Food>::Persistor<Food>(bool temp) : m_class("food")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Stackable>::Persistor<Stackable>(bool temp) : m_class("stackable")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    desc["num"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Structure>::Persistor<Structure>(bool temp) : m_class("structure")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<World>::Persistor<World>(bool temp) : m_class("world")
{
    if (temp) { return; }
    Atlas::Message::Element::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");
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
    // std::string columns;
    // uCharacter(*t, columns);
    // uEntity(*t, columns);
    // Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    // t->clearUpdateFlags();
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

void Persistor<World>::persist(World & t)
{
    hookup(t);
    // We do not create the row in the database. This is handled in a slightly
    // special way.
}
