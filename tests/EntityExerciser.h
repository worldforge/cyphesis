// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

// $Id: EntityExerciser.h,v 1.28 2008-01-17 19:53:39 alriddoch Exp $

#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "common/OperationRouter.h"

#include "common/Add.h"
#include "common/Burn.h"
#include "common/Connect.h"
#include "common/Eat.h"
#include "common/Monitor.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <set>

#include <cassert>

template <class EntityType>
class EntityExerciser {
  protected:
    EntityType & m_ent;
  public:
    explicit EntityExerciser(EntityType & e) : m_ent(e) { }
    virtual ~EntityExerciser() { }

    virtual void dispatchOp(const Atlas::Objects::Operation::RootOperation&op) {
        OpVector ov1;
        m_ent.operation(op, ov1);
        flushOperations(ov1);
    }

    void addAllOperations(std::set<std::string> & ops);

    void runOperations();
    void runConversions();
    void flushOperations(OpVector & ops);
};

template <class EntityType>
inline void EntityExerciser<EntityType>::addAllOperations(std::set<std::string> & ops)
{
    ops.insert("login");
    ops.insert("logout");
    ops.insert("action");
    ops.insert("chop");
    ops.insert("combine");
    ops.insert("create");
    ops.insert("cut");
    ops.insert("delete");
    ops.insert("divide");
    ops.insert("eat");
    ops.insert("burn");
    ops.insert("get");
    ops.insert("imaginary");
    ops.insert("info");
    ops.insert("move");
    ops.insert("nourish");
    ops.insert("set");
    ops.insert("sight");
    ops.insert("sound");
    ops.insert("talk");
    ops.insert("touch");
    ops.insert("tick");
    ops.insert("look");
    ops.insert("setup");
    ops.insert("appearance");
    ops.insert("disappearance");
    ops.insert("use");
    ops.insert("wield");
    ops.insert("error");
}

template <class EntityType>
inline void EntityExerciser<EntityType>::runOperations()
{
    {
        Atlas::Objects::Operation::Login op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Logout op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Combine op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Create op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Delete op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Divide op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Eat op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Get op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Imaginary op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Info op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Move op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Nourish op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Set op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Sight op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Sound op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Talk op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Touch op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Tick op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Look op;
        op->setFrom(m_ent.getId());
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Setup op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Appearance op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Disappearance op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Use op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Wield op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Error op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::RootOperation op;
        dispatchOp(op);
    }
}

template <class EntityType>
inline void EntityExerciser<EntityType>::runConversions()
{
    {
        Atlas::Message::MapType data;

        assert(data.empty());
        m_ent.addToMessage(data);
        assert(!data.empty());
    }

    {
        Atlas::Objects::Entity::Anonymous data;

        assert(data->isDefaultId());
        m_ent.addToEntity(data);
        assert(!data->isDefaultId());
    }
}

template <class EntityType>
inline void EntityExerciser<EntityType>::flushOperations(OpVector & ops)
{
    ops.clear();
}


#endif // TESTS_ENTITY_EXERCISER_H
