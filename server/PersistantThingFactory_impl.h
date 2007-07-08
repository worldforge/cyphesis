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

// $Id: PersistantThingFactory_impl.h,v 1.12 2007-07-08 23:45:12 alriddoch Exp $

#ifndef SERVER_PERSISTANT_THING_FACTORY_IMPL_H
#define SERVER_PERSISTANT_THING_FACTORY_IMPL_H

#include "PersistantThingFactory.h"

template <class T>
void PersistorConnection<T>::persist()
{
    m_p.persist(m_t);
}

template <class T>
ThingFactory<T>::ThingFactory(ThingFactory<T> & o)
{
}

template <class T>
ThingFactory<T>::ThingFactory()
{
}

template <class T>
ThingFactory<T>::~ThingFactory()
{
}

template <class T>
T * ThingFactory<T>::newPersistantThing(const std::string & id, long intId, PersistorBase ** p)
{
    return new T(id, intId);
}

template <class T>
int ThingFactory<T>::populate(Entity &)
{
    return 0;
}

template <class T>
FactoryBase * ThingFactory<T>::duplicateFactory()
{
    FactoryBase * f = new ThingFactory<T>(*this);
    f->m_parent = this;
    return f;
}

template <class T>
PersistantThingFactory<T>::~PersistantThingFactory()
{
    if (m_master) {
        delete &m_p;
    }
}

template <class T>
T * PersistantThingFactory<T>::newPersistantThing(const std::string & id, long intId, PersistorBase ** p)
{
    T * t = new T(id, intId);
    *p = new PersistorConnection<T>(*t, m_p);
    return t;
}

template <class T>
FactoryBase * PersistantThingFactory<T>::duplicateFactory()
{
    FactoryBase * f = new PersistantThingFactory<T>(*this);
    f->m_parent = this;
    return f;
}

template <class T>
ForbiddenThingFactory<T>::~ForbiddenThingFactory()
{
    delete &m_p;
}

template <class T>
T * ForbiddenThingFactory<T>::newPersistantThing(const std::string &, long, PersistorBase **)
{
    return 0;
}

template <class T>
int ForbiddenThingFactory<T>::populate(Entity &)
{
    return 0;
}

template <class T>
FactoryBase * ForbiddenThingFactory<T>::duplicateFactory()
{
    return 0;
}

#endif // SERVER_PERSISTANT_THING_FACTORY_IMPL_H
