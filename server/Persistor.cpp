// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Persistor.h"
#include "Persistor_impl.h"

#include <common/Database.h>

#include <rulesets/Entity.h>
#include <rulesets/Thing.h>
#include <rulesets/Line.h>
#include <rulesets/Area.h>
#include <rulesets/Character.h>
#include <rulesets/Creator.h>
#include <rulesets/Food.h>
#include <rulesets/Plant.h>
#include <rulesets/Stackable.h>
#include <rulesets/Structure.h>

Persistor<Entity>::Persistor() : m_class("entity")
{
    Atlas::Message::Object::MapType desc;
    desc["loc"] = "                                                                                ";
    desc["px"] = 1.0;
    desc["py"] = 1.0;
    desc["pz"] = 1.0;
    desc["ox"] = 1.0;
    desc["oy"] = 1.0;
    desc["oz"] = 1.0;
    desc["ow"] = 1.0;

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
    Database::instance()->registerEntityTable(m_class, desc);
}

Persistor<Thing>::Persistor<Thing>() : m_class("thing")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "entity");
}

Persistor<Line>::Persistor<Line>() : m_class("line")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Area>::Persistor<Area>() : m_class("area")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Character>::Persistor<Character>() : m_class("character")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["sex"] = "        ";
    desc["drunkness"] = 1.0;
    desc["food"] = 1.0;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Creator>::Persistor<Creator>() : m_class("creator")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "character");
}

Persistor<Plant>::Persistor<Plant>() : m_class("plant")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["fruits"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Food>::Persistor<Food>() : m_class("food")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Stackable>::Persistor<Stackable>() : m_class("stackable")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    desc["num"] = 1;
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

Persistor<Structure>::Persistor<Structure>() : m_class("structure")
{
    Atlas::Message::Object::MapType desc;
    // FIXME Sort out attributes
    Database::instance()->registerEntityTable(m_class, desc, "thing");
}

void Persistor<Character>::update(Character & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uCharacter(t, columns);
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

void Persistor<Creator>::update(Creator & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uCharacter(t, columns);
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

void Persistor<Line>::update(Line & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uLine(t, columns);
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

void Persistor<Area>::update(Area & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uArea(t, columns);
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

void Persistor<Plant>::update(Plant & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uPlant(t, columns);
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

void Persistor<Character>::persist(Character & t)
{
    std::cout << "Persistor::persist<" << m_class << ">(" << t.getId()
              << ")" << std::endl << std::flush;
    t.updated.connect(SigC::bind<Character &>(SigC::slot(*this,
                                                 &Persistor<Character>::update),
                                      t));
    t.destroyed.connect(SigC::bind<Character &>(SigC::slot(*this,
                                                   &Persistor<Character>::remove),
                                        t));
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    cCharacter(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}

void Persistor<Creator>::persist(Creator & t)
{
    std::cout << "Persistor::persist<" << m_class << ">(" << t.getId()
              << ")" << std::endl << std::flush;
    t.updated.connect(SigC::bind<Creator &>(SigC::slot(*this,
                                                 &Persistor<Creator>::update),
                                      t));
    t.destroyed.connect(SigC::bind<Creator &>(SigC::slot(*this,
                                                   &Persistor<Creator>::remove),
                                        t));
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    cCharacter(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}
