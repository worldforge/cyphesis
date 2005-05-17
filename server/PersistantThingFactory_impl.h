
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
PersistantThingFactory<T>::~PersistantThingFactory()
{
    if (m_master) {
        delete &m_p;
    }
}

template <class T>
T * PersistantThingFactory<T>::newThing(const std::string & id)
{
    return new T(id);
}

template <class T>
T * PersistantThingFactory<T>::newPersistantThing(const std::string &id, PersistorBase ** p)
{
    T * t = new T(id);
    *p = new PersistorConnection<T>(*t, m_p);
    return t;
}

template <class T>
FactoryBase * PersistantThingFactory<T>::duplicateFactory()
{
    return new PersistantThingFactory<T>(*this);
}

template <class T>
ForbiddenThingFactory<T>::~ForbiddenThingFactory()
{
    delete &m_p;
}

template <class T>
T * ForbiddenThingFactory<T>::newThing(const std::string &)
{
    return 0;
}

template <class T>
T * ForbiddenThingFactory<T>::newPersistantThing(const std::string &, PersistorBase **)
{
    return 0;
}

template <class T>
FactoryBase * ForbiddenThingFactory<T>::duplicateFactory()
{
    return 0;
}

#endif // SERVER_PERSISTANT_THING_FACTORY_IMPL_H
