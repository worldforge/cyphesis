
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_PERSISTANT_THING_FACTORY_IMPL_H
#define SERVER_PERSISTANT_THING_FACTORY_IMPL_H

#include "PersistantThingFactory.h"

template <class T>
void PersistorConnection<T>::persist()
{
    m_p.persist(m_t);
}

template <class T>
T * PersistantThingFactory<T>::newThing()
{
    return new T();
}

template <class T>
T * PersistantThingFactory<T>::newPersistantThing(PersistorBase ** p)
{
    T * t = new T();
    *p = new PersistorConnection<T>(*t, m_p);
    return t;
}

template <class T>
FactoryBase * PersistantThingFactory<T>::duplicateFactory()
{
    return new PersistantThingFactory<T>(*this);
}

#endif // SERVER_PERSISTANT_THING_FACTORY_IMPL_H
