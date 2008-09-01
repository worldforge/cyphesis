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

// $Id$

#ifndef SERVER_PERSISTANT_THING_FACTORY_IMPL_H
#define SERVER_PERSISTANT_THING_FACTORY_IMPL_H

#include "PersistantThingFactory.h"

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
T * ThingFactory<T>::newPersistantThing(const std::string & id,
                                        long intId,
                                        PersistorBase ** p)
{
    ++m_createdCount;
    return new T(id, intId);
}

template <class T>
int ThingFactory<T>::populate(Entity &)
{
    return 0;
}

template <class T>
EntityKit * ThingFactory<T>::duplicateFactory()
{
    EntityKit * f = new ThingFactory<T>(*this);
    f->m_parent = this;
    return f;
}

template <class T>
PersistantThingFactory<T>::~PersistantThingFactory()
{
}

template <class T>
T * PersistantThingFactory<T>::newPersistantThing(const std::string & id,
                                                  long intId,
                                                  PersistorBase ** p)
{
    ++this->m_createdCount;
    T * t = new T(id, intId);
    // FIXME Return a PersistorConnection
    return t;
}

template <class T>
EntityKit * PersistantThingFactory<T>::duplicateFactory()
{
    EntityKit * f = new PersistantThingFactory<T>(*this);
    f->m_parent = this;
    return f;
}

template <class T>
ForbiddenThingFactory<T>::~ForbiddenThingFactory()
{
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
EntityKit * ForbiddenThingFactory<T>::duplicateFactory()
{
    return 0;
}

#endif // SERVER_PERSISTANT_THING_FACTORY_IMPL_H
