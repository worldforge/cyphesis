// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef RULESETS_CONTAINER_H
#define RULESETS_CONTAINER_H

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

class Container {
  protected:
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

        virtual Container_const_iterator & operator++() = 0;
        virtual bool operator==(const Container_const_iterator &) = 0;
        virtual Entity * operator*() const = 0;
    };
  public:
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

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;

    virtual int size() = 0;
    virtual bool empty() = 0;
    virtual void insert(Entity *) = 0;
    virtual void erase(Entity *) = 0;
    virtual void reParent() = 0;
    virtual void addToMessage(const std::string &, Atlas::Message::MapType &) const = 0;
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
};
#endif

class StdContainer : public Container {
  public:
    class StdContainer_const_iterator : public Container_const_iterator {
      private:
        EntitySet::const_iterator m_iter;
      public:
        StdContainer_const_iterator(EntitySet::const_iterator I);
        virtual ~StdContainer_const_iterator();

        virtual StdContainer_const_iterator & operator++();
        virtual bool operator==(const Container_const_iterator &);
        virtual Entity * operator*() const;
    };
  protected:
    EntitySet m_entities;
  public:
    StdContainer();
    virtual ~StdContainer();

    virtual const_iterator begin() const;
    virtual const_iterator end() const;

    virtual int size();
    virtual bool empty();
    virtual void insert(Entity *);
    virtual void erase(Entity *);
    virtual void reParent();
    virtual void addToMessage(const std::string &, Atlas::Message::MapType &) const;
};

#endif // RULESETS_CONTAINER_H
