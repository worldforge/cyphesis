// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Container.h"
#include "Entity.h"

#include <Atlas/Message/Element.h>

Container::Container_const_iterator::Container_const_iterator() : m_refCount(0)
{
}

Container::Container_const_iterator::~Container_const_iterator()
{
}

Container::Container()
{
}

Container::~Container()
{
}

Container::const_iterator StdContainer::begin() const
{
    const_iterator ci(new StdContainer_const_iterator(m_entities.begin()));
    return ci;
}

Container::const_iterator StdContainer::end() const
{
    const_iterator ci(new StdContainer_const_iterator(m_entities.end()));
    return ci;
}

#if 0
NonContainer::NonContainer()
{
}

NonContainer::~NonContainer()
{
}

int NonContainer::size()
{
    return 0;
}

#if 0
// There should be a link error if these are ever used without the
// implementation issues being sorted out. Exactly how do we implement it
// so that this entity cannot be a container?

void NonContainer::insert(Entity *)
{
    // Error
}

void NonContainer::erase(Entity *)
{
    // Error
}

void NonContainer::reParent()
{
    // Error
}
#endif

void NonContainer::addToMessage(const std::string &, Atlas::Message::MapType &)
{
    // Do nothing, as we never have any contents.
}
#endif

StdContainer::StdContainer_const_iterator::StdContainer_const_iterator(EntitySet::const_iterator I) : m_iter(I)
{
}

StdContainer::StdContainer_const_iterator::~StdContainer_const_iterator()
{
}

StdContainer::StdContainer_const_iterator & StdContainer::StdContainer_const_iterator::operator++()
{
    ++m_iter;
    return *this;
}

bool StdContainer::StdContainer_const_iterator::operator==(const Container_const_iterator & o)
{
    const StdContainer::StdContainer_const_iterator * other = dynamic_cast<const StdContainer::StdContainer_const_iterator *>(&o);
    if (other == 0) {
        return false;
    }
    return (m_iter == other->m_iter);
}

Entity * StdContainer::StdContainer_const_iterator::operator*() const
{
    return (*m_iter);
}

StdContainer::StdContainer()
{
}

StdContainer::~StdContainer()
{
}

int StdContainer::size()
{
    return m_entities.size();
}

bool StdContainer::empty()
{
    return m_entities.empty();
}

void StdContainer::insert(Entity * ent)
{
    m_entities.insert(ent);
}

void StdContainer::erase(Entity * ent)
{
    m_entities.erase(ent);
}

void StdContainer::reParent()
{
    // WTF
}

void StdContainer::addToMessage(const std::string & key,
                                Atlas::Message::MapType & map) const
{
    Atlas::Message::Element & e = map[key];
    e = Atlas::Message::ListType();
    Atlas::Message::ListType & contlist = e.asList();
    EntitySet::const_iterator Iend = m_entities.end();
    for (EntitySet::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        contlist.push_back((*I)->getId());
    }
}
