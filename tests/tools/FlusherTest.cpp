// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "tools/Flusher.h"

#include "tools/ObjectContext.h"

#include "common/compose.hpp"
#include "common/operations/Tick.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

class test_ObjectContext : public ObjectContext {
  public:
    std::string m_id;
    test_ObjectContext(const std::string & id) :
          ObjectContext(*(Interactive*)0), m_id(id) { }
    virtual bool accept(const Atlas::Objects::Operation::RootOperation&) const
    {
        return false;
    }

    virtual int dispatch(const Atlas::Objects::Operation::RootOperation&)
    {
        return 0;
    }

    virtual std::string repr() const
    {
        return "test_context";
    }

    virtual bool checkContextCommand(const struct command *)
    {
        return false;
    }

    virtual void setFromContext(const Atlas::Objects::Operation::RootOperation&op)
    {
        op->setFrom(m_id);
    }
};

using std::shared_ptr;

int main()
{
    shared_ptr<ObjectContext> test_context(new test_ObjectContext("1"));


    {
        ClientTask * tf = new Flusher(test_context);

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        tf->setup("oak", ret);
        assert(!ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Get op;
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Sight op;
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Sight op;
        op->setArgs1(Atlas::Objects::Entity::Anonymous());
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Sight op;
        Atlas::Objects::Entity::Anonymous ent;
        ent->setId("2");
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Sight op;
        Atlas::Objects::Entity::Anonymous ent;
        ent->setId("2");
        ent->setParent("oak");
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        tf->setup("oak", ret);
        assert(!ret.empty());
        assert(ret.size() == 1);
        ret.clear();
        Atlas::Objects::Operation::Sight op;
        Atlas::Objects::Entity::Anonymous ent;
        ent->setId("2");
        ent->setParent("oak");
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(!ret.empty());
        assert(ret.size() == 2);

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Tick op;
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Tick op;
        Atlas::Objects::Entity::Anonymous ent;
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Tick op;
        Atlas::Objects::Entity::Anonymous ent;
        ent->setName("725e66b2-2e35-4eb8-b3af-3de5691bf48a");
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);

        OpVector ret;
        Atlas::Objects::Operation::Tick op;
        Atlas::Objects::Entity::Anonymous ent;
        ent->setName("flusher");
        op->setArgs1(ent);
        tf->operation(op, ret);
        assert(!ret.empty());

        delete tf;
    }

    {
        ClientTask * tf = new Flusher(test_context);
        assert(!tf->isComplete());

        OpVector ret;
        Atlas::Objects::Operation::Unseen op;
        tf->operation(op, ret);
        assert(ret.empty());
        assert(tf->isComplete());

        delete tf;
    }
}

// stubs

#include "common/log.h"


namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = 1000;
} } }

#include "../stubs/common/stublog.h"
