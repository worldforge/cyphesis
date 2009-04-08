// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

#ifndef MODULES_ENTITY_REF_H
#define MODULES_ENTITY_REF_H

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

class Entity;

class EntityRef : public sigc::trackable
{
  private:
    Entity * m_inner;

    void onEntityDeleted();

    void setup();
  public:
    EntityRef() : m_inner(NULL)
    {
    }

    explicit EntityRef(Entity*);

    ~EntityRef()
    {
    }

    EntityRef(const EntityRef& ref);

    EntityRef& operator=(const EntityRef& ref);

    Entity& operator*() const
    {
        return *m_inner;
    }

    Entity* operator->() const
    {
        return m_inner;
    }

    Entity* get() const
    {
        return m_inner;
    }

    bool operator==(const EntityRef& e) const
    {
        return (m_inner == e.m_inner);
    }

    bool operator==(const Entity* e) const
    {
        return (m_inner == e);
    }

    bool operator<(const EntityRef& e) const
    {
        return (m_inner < e.m_inner);
    }

    sigc::signal<void> Changed;
};

#endif // MODULES_ENTITY_REF_H
