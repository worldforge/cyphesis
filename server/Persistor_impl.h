// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_IMPL_H
#define SERVER_PERSISTOR_IMPL_H

#include "common/Database.h"
#include "common/stringstream.h"

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

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
        q << "px = " << t.m_location.m_pos.x()
          << ", py = " << t.m_location.m_pos.y()
          << ", pz = " << t.m_location.m_pos.z();
    }
    if (t.getUpdateFlags() & a_orient) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "ox = " << t.m_location.m_orientation.vector().x()
          << ", oy = " << t.m_location.m_orientation.vector().y()
          << ", oz = " << t.m_location.m_orientation.vector().z()
          << ", ow = " << t.m_location.m_orientation.scalar();
    }
    if (t.getUpdateFlags() & a_bbox) {
        if (!empty) { q << ", "; } else { empty = false; }
        if (t.m_location.m_bBox.isValid()) {
            q << "hasBox = 't'"
              << ", bnx = " << t.m_location.m_bBox.lowCorner().x()
              << ", bny = " << t.m_location.m_bBox.lowCorner().y()
              << ", bnz = " << t.m_location.m_bBox.lowCorner().z()
              << ", bfx = " << t.m_location.m_bBox.highCorner().x()
              << ", bfy = " << t.m_location.m_bBox.highCorner().y()
              << ", bfz = " << t.m_location.m_bBox.highCorner().z();
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
    if (t.getUpdateFlags() & a_drunk) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "drunkness = " << t.getDrunkness();
    }
    if (t.getUpdateFlags() & a_sex) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "sex = '" << t.getSex() << "'";
    }
    if (t.getUpdateFlags() & a_food) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "food = " << t.getFood();
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
      << t.m_location.m_pos.x() << cs
      << t.m_location.m_pos.y() << cs
      << t.m_location.m_pos.z() << cs
      << t.m_location.m_orientation.vector().x() << cs
      << t.m_location.m_orientation.vector().y() << cs
      << t.m_location.m_orientation.vector().z() << cs
      << t.m_location.m_orientation.scalar() << cs
      << (t.m_location.m_bBox.isValid() ? "'t'" : "'f'") << cs
      << t.m_location.m_bBox.lowCorner().x() << cs
      << t.m_location.m_bBox.lowCorner().y() << cs
      << t.m_location.m_bBox.lowCorner().z() << cs
      << t.m_location.m_bBox.highCorner().x() << cs
      << t.m_location.m_bBox.highCorner().y() << cs
      << t.m_location.m_bBox.highCorner().z() << cs
      << t.getStatus() << cs
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
    c += "drunkness, sex, food";

    std::stringstream q;
    q << t.getDrunkness() << cs
      << sq << t.getSex() << sq << cs
      << t.getFood();

    if (!v.empty()) {
        v += cs;
    }
    v += q.str();
}

template <class T>
void Persistor<T>::hookup(T & t)
{
    t.updated.connect(SigC::bind<T*>(SigC::slot(*this, &Persistor<T>::update),
                                      &t));
    t.destroyed.connect(SigC::bind<T*>(SigC::slot(*this, &Persistor<T>::remove),
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
