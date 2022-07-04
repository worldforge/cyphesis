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


#ifndef SERVER_ENTITY_FACTORY_IMPL_H
#define SERVER_ENTITY_FACTORY_IMPL_H

#include "EntityFactory.h"

template<class T>
EntityFactory<T>::EntityFactory(EntityFactory<T>& o)
{
}

template<class T>
EntityFactory<T>::EntityFactory() = default;

template<class T>
EntityFactory<T>::~EntityFactory() = default;

template<class T>
Ref<Entity> EntityFactory<T>::newEntity(RouterId id,
                                               const Atlas::Objects::Entity::RootEntity& attributes)
{
    ++m_createdCount;
    //Important that we create a ref as soon as possible, so we have a positive ref count.
    Ref<T> thing(new T(id));
    initializeEntity(*thing, attributes);
    return thing;
}

template<class T>
std::unique_ptr<EntityFactoryBase> EntityFactory<T>::duplicateFactory()
{
    auto f = std::unique_ptr<EntityFactory<T>>(new EntityFactory<T>(*this));
    // Copy the defaults to the parent
    f->m_attributes = this->m_attributes;
    f->m_parent = this;
    return f;
}

#endif // SERVER_ENTITY_FACTORY_IMPL_H
