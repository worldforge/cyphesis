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


#ifndef MODULES_WEAK_ENTITY_REF_H
#define MODULES_WEAK_ENTITY_REF_H

#include "Ref.h"
#include <sigc++/connection.h>
#include <sigc++/signal.h>

class LocatedEntity;

class WeakEntityRef
{
  private:
    LocatedEntity * m_inner;

    sigc::connection m_connection;

    void onEntityDeleted();

    void setup();
  public:
    WeakEntityRef() : m_inner(nullptr)
    {
    }

    explicit WeakEntityRef(const Ref<LocatedEntity>&);
    explicit WeakEntityRef(LocatedEntity*);

    ~WeakEntityRef();

    WeakEntityRef(const WeakEntityRef& ref);

    WeakEntityRef& operator=(const WeakEntityRef& ref);

    WeakEntityRef& operator=(LocatedEntity* ref);
    WeakEntityRef& operator=(const Ref<LocatedEntity>& ref);

    LocatedEntity& operator*() const noexcept
    {
        return *m_inner;
    }

    LocatedEntity* operator->() const noexcept
    {
        return m_inner;
    }

    LocatedEntity* get() const noexcept
    {
        return m_inner;
    }

    bool operator==(const WeakEntityRef& e) const noexcept
    {
        return (m_inner == e.m_inner);
    }

    bool operator==(const LocatedEntity* e) const noexcept
    {
        return (m_inner == e);
    }

    bool operator<(const WeakEntityRef& e) const noexcept
    {
        return (m_inner < e.m_inner);
    }

    bool operator!() const noexcept {
        return this->m_inner == nullptr;
    }

    explicit operator bool () const noexcept
    {
        return !this->operator!();
    }

    sigc::signal<void()> Changed;
};

#endif // MODULES_WEAK_ENTITY_REF_H
