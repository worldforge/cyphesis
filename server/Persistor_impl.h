// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTOR_IMPL_H
#define SERVER_PERSISTOR_IMPL_H

#include <common/Database.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

template <class T>
class Persistor;

template <class T>
void Persistor<T>::persist(T & t)
{
    std::cout << "Persistor::persist<" << m_class << ">(" << t.getId()
              << ")" << std::endl << std::flush;
    t.updated.connect(SigC::bind<T &>(SigC::slot(*this,
                                                 &Persistor<T>::update),
                                      t));
    t.destroyed.connect(SigC::bind<T &>(SigC::slot(*this,
                                                   &Persistor<T>::remove),
                                        t));
    Database::instance()->createEntityRow(m_class, t.getId(), "");
}

template <class T>
void Persistor<T>::update(T & t)
{
    std::cout << "Persistor::update<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    Database::instance()->updateEntityRow(m_class, t.getId(), "");
}

template <class T>
void Persistor<T>::remove(T & t)
{
    std::cout << "Persistor::remove<" << m_class << ">(" << t.getId() << ")"
              << std::endl << std::flush;
    Database::instance()->removeEntityRow(m_class, t.getId());
}

#endif // SERVER_PERSISTOR_IMPL_H
