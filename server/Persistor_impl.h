// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_IMPL_H
#define SERVER_PERSISTOR_IMPL_H

#include <common/Database.h>
#include <common/stringstream.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

template <class T>
void Persistor<T>::uEntity(Entity & t, std::string & c)
{
    std::stringstream q;
    bool empty = c.empty();
    if (t.getUpdateFlags() & a_loc) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "loc = '" << t.location.ref->getId() << "'";
    }
    if (t.getUpdateFlags() & a_pos) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "px = " << t.location.coords.X()
          << ", py = " << t.location.coords.Y()
          << ", pz = " << t.location.coords.Z();
    }
    if (t.getUpdateFlags() & a_orient) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "ox = " << t.location.orientation.X()
          << ", oy = " << t.location.orientation.Y()
          << ", oz = " << t.location.orientation.Z()
          << ", ow = " << t.location.orientation.W();
    }
    if (t.getUpdateFlags() & a_bbox) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "bnx = " << t.location.bBox.nearPoint().X()
          << ", bny = " << t.location.bBox.nearPoint().Y()
          << ", bnz = " << t.location.bBox.nearPoint().Z()
          << ", bfx = " << t.location.bBox.farPoint().X()
          << ", bfy = " << t.location.bBox.farPoint().Y()
          << ", bfz = " << t.location.bBox.farPoint().Z();
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

    if (!empty) {
        if (!empty) { q << ", "; } else { empty = false; }
        q << "seq = " << t.getSeq();
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
        q << "drunk = " << t.getDrunkness();
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
    c += "loc, px, py, pz, ox, oy, oz, ow, bnx, bny, bnz, bfx, bfx, bfz, status, name, mass, seq";

    std::stringstream q;
    q << sq << t.location.ref->getId() << sq << cs
      << t.location.coords.X() << cs
      << t.location.coords.Y() << cs
      << t.location.coords.Z() << cs
      << t.location.orientation.X() << cs
      << t.location.orientation.Y() << cs
      << t.location.orientation.Z() << cs
      << t.location.orientation.W() << cs
      << t.location.bBox.nearPoint().X() << cs
      << t.location.bBox.nearPoint().Y() << cs
      << t.location.bBox.nearPoint().Z() << cs
      << t.location.bBox.farPoint().X() << cs
      << t.location.bBox.farPoint().Y() << cs
      << t.location.bBox.farPoint().Z() << cs
      << t.getStatus() << cs
      << sq << t.getName() << sq << cs
      << t.getMass() << cs
      << t.getSeq();

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
    c += "drunk, sex, food";

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
void Persistor<T>::persist(T & t)
{
    std::cout << "Persistor::persist<T(" << m_class << ")>(" << t.getId()
              << ")" << std::endl << std::flush;
    t.updated.connect(SigC::bind<T &>(SigC::slot(*this,
                                                 &Persistor<T>::update),
                                      t));
    t.destroyed.connect(SigC::bind<T &>(SigC::slot(*this,
                                                   &Persistor<T>::remove),
                                        t));
    std::string columns;
    std::string values;
    cEntity(t, columns, values);
    Database::instance()->createEntityRow(m_class, t.getId(), columns, values);
}

template <class T>
void Persistor<T>::update(T & t)
{
    std::cout << "Persistor::update<T(" << m_class << ")>(" << t.getId() << ")"
              << std::endl << std::flush;
    std::string columns;
    uEntity(t, columns);
    Database::instance()->updateEntityRow(m_class, t.getId(), columns);
    t.clearUpdateFlags();
}

template <class T>
void Persistor<T>::remove(T & t)
{
    std::cout << "Persistor::remove<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    Database::instance()->removeEntityRow(m_class, t.getId());
}

#endif // SERVER_PERSISTOR_IMPL_H
