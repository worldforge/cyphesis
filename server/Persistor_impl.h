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

#ifndef SERVER_PERSISTOR_IMPL_H
#define SERVER_PERSISTOR_IMPL_H

#include "rulesets/Entity.h"
#include "rulesets/Character.h"
#include "rulesets/Plant.h"

#include "common/Database.h"

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

#include <sstream>

template <class T>
void Persistor<T>::uEntity(Entity & t, std::string & c)
{
    std::stringstream q;
    bool empty = c.empty();
    if (t.getUpdateFlags() & a_loc) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "loc = '" << t.m_location.m_loc->getId() << "'";
    }
    if (t.getUpdateFlags() & a_pos) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "px = " << t.m_location.pos().x()
          << ", py = " << t.m_location.pos().y()
          << ", pz = " << t.m_location.pos().z();
    }
    if (t.getUpdateFlags() & a_orient) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "ox = " << t.m_location.orientation().vector().x()
          << ", oy = " << t.m_location.orientation().vector().y()
          << ", oz = " << t.m_location.orientation().vector().z()
          << ", ow = " << t.m_location.orientation().scalar();
    }
    if (t.getUpdateFlags() & a_bbox) {
        if (!empty) { q << ", "; } else { empty = false; }
        if (t.m_location.bBox().isValid()) {
            q << "hasBox = 't'"
              << ", bnx = " << t.m_location.bBox().lowCorner().x()
              << ", bny = " << t.m_location.bBox().lowCorner().y()
              << ", bnz = " << t.m_location.bBox().lowCorner().z()
              << ", bfx = " << t.m_location.bBox().highCorner().x()
              << ", bfy = " << t.m_location.bBox().highCorner().y()
              << ", bfz = " << t.m_location.bBox().highCorner().z();
        } else {
            q << "hasBox = 'f'";
        }
    }
    if (t.getUpdateFlags() & a_cont) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "cont = " << t.m_contains.size();
    }
    if (t.getUpdateFlags() & a_status) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "status = " << t.getStatus();
    }
    if (t.getUpdateFlags() & a_name) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "name = '" << t.getName() << "'";
    }
    if (t.getUpdateFlags() & a_mass) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "mass = " << t.getMass();
    }
    if (t.getUpdateFlags() & a_attr) {
        if (!empty) { q << ", "; } else { empty = false; }
        std::string aString;
        Database::instance()->encodeObject(t.getAttributes(), aString);
        q << "attributes = '" << aString << "'";
    }
    if (!empty) {
        q << ", seq = " << t.getSeq();
        c += q.str();
    }
}

template <class T>
void Persistor<T>::uCharacter(Character & t, std::string & c)
{
    std::stringstream q;
    bool empty = c.empty();
    if (t.getUpdateFlags() & a_sex) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "sex = '" << t.getSex() << "'";
    }
    if (t.getUpdateFlags() & a_food) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "food = " << t.getFood();
    }
    if (t.getUpdateFlags() & a_rwield) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "right_hand_wield = '" << t.getRightHandWield() << "'";
    }
    if (!empty) {
        c += q.str();
    }
}

template <class T>
void Persistor<T>::uLine(Line & t, std::string & c)
{
}

template <class T>
void Persistor<T>::uArea(Area & t, std::string & c)
{
}

template <class T>
void Persistor<T>::uPlant(Plant & t, std::string & c)
{
    std::stringstream q;
    bool empty = c.empty();
    if (t.getUpdateFlags() & a_fruit) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "fruits = " << t.getFruits();
        q << ", fruitName = '" << t.getFruitName() << "'";
        q << ", fruitChance = " << t.getFruitChance();
        q << ", sizeAdult = " << t.getSizeAdult();
    }
    if (!empty) {
        c += q.str();
    }
}

template <class T>
void Persistor<T>::cEntity(Entity & t, std::string & c, std::string & v)
{
    const char * cs = ", ";
    const char * sq = "'";
    if (!c.empty()) {
        c += cs;
    }
    c += "class, type, loc, cont, px, py, pz, ox, oy, oz, ow, hasBox, bnx, bny, bnz, bfx, bfy, bfz, status, name, mass, seq, attributes";

    std::stringstream q;
    q << sq << m_class << sq << cs
      << sq << t.getType() << sq << cs
      << sq << t.m_location.m_loc->getId() << sq << cs
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

template <class T>
void Persistor<T>::cCharacter(Character & t, std::string & c, std::string & v)
{
    const char * cs = ", ";
    const char * sq = "'";
    if (!c.empty()) {
        c += cs;
    }
    c += "sex, food, right_hand_wield";

    std::stringstream q;
    q << sq << t.getSex() << sq << cs
      << t.getFood() << cs
      << sq << t.getRightHandWield() << sq;

    if (!v.empty()) {
        v += cs;
    }
    v += q.str();
}

template <class T>
void Persistor<T>::cPlant(Plant & p, std::string & c, std::string & v)
{
    const char * cs = ", ";
    const char * sq = "'";
    if (!c.empty()) {
        c += cs;
    }
    c += "fruits, fruitName, fruitChance, sizeAdult";

    std::stringstream q;
    q << p.getFruits() << cs
      << sq << p.getFruitName() << sq << cs
      << p.getFruitChance() << cs
      << p.getSizeAdult();

    if (!v.empty()) {
        v += cs;
    }
    v += q.str();
}

template <class T>
void Persistor<T>::hookup(T & t)
{
    t.updated.connect(sigc::bind(sigc::mem_fun(this, &Persistor<T>::update),
                                 &t));
    t.destroyed.connect(sigc::bind(sigc::mem_fun(this, &Persistor<T>::remove),
                                   &t));
}

template <class T>
void Persistor<T>::persist(T & t)
{
    hookup(t);
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}

template <class T>
void Persistor<T>::update(T * const t)
{
    std::string columns;
    uEntity(*t, columns);
    Database::instance()->updateEntityRow(m_class, t->getId(), columns);
    t->clearUpdateFlags();
}

template <class T>
void Persistor<T>::remove(T * const t)
{
    Database::instance()->removeEntityRow(m_class, t->getId());
}

#endif // SERVER_PERSISTOR_IMPL_H
