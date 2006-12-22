// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

// $Id: Container.h,v 1.8 2006-12-22 02:14:44 alriddoch Exp $

#ifndef RULESETS_CONTAINER_H
#define RULESETS_CONTAINER_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <set>
#include <map>
#include <vector>

class Entity;

typedef std::set<Entity *> EntitySet;

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
        typedef std::vector<Element> ListType;
    }
}

/// \brief Base class for handling the entity containership tree
///
/// This class replaces STL set in in-game entities so that contents can
/// be handled more flexibly.
class Container {
  protected:
    /// \brief Base class for underlying implementation of Container iterators.
    class Container_const_iterator {
      private:
        int m_refCount;
      public:
        Container_const_iterator();
        virtual ~Container_const_iterator();

        void incRef() {
            ++m_refCount;
        }

        void decRef() {
            if (m_refCount == 0) {
                delete this;
                return;
            }
            --m_refCount;
        }

        /// \brief Increment the iterator to point to the next entity
        virtual Container_const_iterator & operator++() = 0;
        /// \brief Check if the iterator points to the same entity
        virtual bool operator==(const Container_const_iterator &) = 0;
        /// \brief Dereference the iterator to get the entity
        virtual Entity * operator*() const = 0;
    };
  public:
    /// \brief Iterator for Container
    ///
    /// Mimics the STL iterator API, but uses the Container_const_iterator
    /// base class to deal with the fact that Container is an abstract class,
    /// and its children all work diferently.
    class const_iterator {
      private:
        Container_const_iterator * m_i;

        void incRef() const {
            if (m_i != 0) {
                m_i->incRef();
            }
        }

        void decRef() const {
            if (m_i != 0) {
                m_i->decRef();
            }
        }
      public:
        const_iterator() : m_i(0) { }
        explicit const_iterator(Container_const_iterator * i) : m_i(i) { }
        const_iterator(const const_iterator & o) : m_i(o.m_i) {
            incRef();
        }
        ~const_iterator() {
            decRef();
        }
        const_iterator & operator=(const const_iterator & o) {
            if (m_i != o.m_i) {
                decRef();
                m_i = o.m_i;
                incRef();
            }
            return *this;
        }

        const_iterator & operator++() {
            ++(*m_i);
            return *this;
        }
        bool operator==(const const_iterator & I) { return (*m_i)==(*I.m_i); }
        bool operator!=(const const_iterator & I) { return !operator==(I); }
        Entity * operator*() const { return **m_i; }
    };
  private:
    // Private and un-implemented, to make sure slicing is impossible.
    Container(const Container &);
    const Container & operator=(const Container &);
  public:
    Container();
    virtual ~Container();

    /// \brief Return an iterator that points to the start of the container
    virtual const_iterator begin() const = 0;
    /// \brief Return an iterator that points after the end of the container
    virtual const_iterator end() const = 0;

    /// \brief Query the number of members in the container
    virtual int size() const = 0;
    /// \brief Query whether the container is empty
    virtual bool empty() const = 0;
    /// \brief Insert a member into the container
    virtual void insert(Entity *) = 0;
    /// \brief Remove a member from the container
    virtual void erase(Entity *) = 0;
    /// \brief Can't remember what this was to do
    virtual void reParent() = 0;
    /// \brief Add the CONTAINS attribute to the message for this container
    virtual void addToMessage(const std::string &, Atlas::Message::MapType &) const = 0;
    /// \brief Add the CONTAINS attribute to the entity for this container
    virtual void addToEntity(const std::string &, const Atlas::Objects::Entity::RootEntity &) const = 0;
};

#if 0
class NonContainer : public Container {
  public:
    NonContainer();
    virtual ~NonContainer();

    virtual int size();
    virtual void insert(Entity *);
    virtual void erase(Entity *);
    virtual void reParent();
    virtual void addToMessage(const std::string &, Atlas::Message::MapType &) const;
    virtual void addToEntity(const std::string &, Atlas::Objects::Entity::RootEntity &) const = 0;
};
#endif

/// \brief Class to handle the simplest kind of containership
///
/// Directly replaces the old STL set m_contents member of Entity as it
/// uses STL set as its store.
class StdContainer : public Container {
  protected:
    /// \brief Class for underlying implementation of StdContainer iterators.
    class StdContainer_const_iterator : public Container_const_iterator {
      private:
        /// \brief STL iterator for the underlying STL container.
        EntitySet::const_iterator m_iter;
      public:
        StdContainer_const_iterator(EntitySet::const_iterator I);
        virtual ~StdContainer_const_iterator();

        virtual StdContainer_const_iterator & operator++();
        virtual bool operator==(const Container_const_iterator &);
        virtual Entity * operator*() const;
    };
  protected:
    /// \brief Storage for the entities in this container
    EntitySet m_entities;
  public:
    StdContainer();
    virtual ~StdContainer();

    virtual const_iterator begin() const;
    virtual const_iterator end() const;

    virtual int size() const;
    virtual bool empty() const;
    virtual void insert(Entity *);
    virtual void erase(Entity *);
    virtual void reParent();
    virtual void addToMessage(const std::string &, Atlas::Message::MapType &) const;
    virtual void addToEntity(const std::string &, const Atlas::Objects::Entity::RootEntity &) const;
};

#endif // RULESETS_CONTAINER_H
