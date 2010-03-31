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

// $Id$

#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "rulesets/LocatedEntity.h"

#include "common/OperationRouter.h"
#include "common/TypeNode.h"
#include "common/Property.h"

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

class LocatedEntityTest : public LocatedEntity {
  public:
    LocatedEntityTest(const std::string & id, int iid) :
        LocatedEntity(id, iid) { }

    virtual void operation(const Operation &, OpVector &) { /* REMOVE */ }
};

class EntityExerciser {
  protected:
    LocatedEntity & m_ent;
    std::set<int> attr_types;
  public:
    explicit EntityExerciser(LocatedEntity & e) : m_ent(e) {
        if (e.getIntId() == 0) {
            e.makeContainer();
            assert(e.m_contains != 0);
        } else {
            e.m_location.m_loc = new LocatedEntityTest("0", 0);
            e.m_location.m_loc->makeContainer();
            assert(e.m_location.m_loc->m_contains != 0);
            e.m_location.m_loc->m_contains->insert(&e);
        }
        if (e.getType() == 0) {
            TypeNode * test_type = new TypeNode;
            test_type->name() = "test_type";
            test_type->defaults()["test_default"] = new SoftProperty;
            e.setType(test_type);
        }
        attr_types.insert(Atlas::Message::Element::TYPE_INT);
        attr_types.insert(Atlas::Message::Element::TYPE_FLOAT);
        attr_types.insert(Atlas::Message::Element::TYPE_STRING);
        attr_types.insert(Atlas::Message::Element::TYPE_MAP);
        attr_types.insert(Atlas::Message::Element::TYPE_LIST);
    }
    virtual ~EntityExerciser() { }

    bool checkAttributes(const std::set<std::string> & attr_names);
    bool checkProperties(const std::set<std::string> & prop_names);

    bool fullAttributeTest(const Atlas::Message::MapType & attr_data);

    virtual void dispatchOp(const Atlas::Objects::Operation::RootOperation&op) {
        OpVector ov1;
        m_ent.operation(op, ov1);
        flushOperations(ov1);
    }

    void addAllOperations(std::set<std::string> & ops);

    virtual void runOperations();
    void runConversions();
    void flushOperations(OpVector & ops);
};

inline bool EntityExerciser::checkAttributes(const std::set<std::string> & attr_names)
{
    Atlas::Message::Element null;
    bool res = true;
    std::set<std::string>::const_iterator I = attr_names.begin();
    std::set<std::string>::const_iterator Iend = attr_names.end();
    for (; I != Iend; ++I) {
        if (!this->m_ent.getAttr(*I, null)) {
            std::cerr << "Entity does not have \"" << *I << "\" attribute."
                      << std::endl << std::flush;
            res = false;
        }
        if (this->m_ent.getProperty(*I) == NULL) {
            std::cerr << "Entity does not have \"" << *I << "\" property."
                      << std::endl << std::flush;
            res = false;
        }
        std::set<int>::const_iterator J = attr_types.begin();
        std::set<int>::const_iterator Jend = attr_types.end();
        for (; J != Jend; ++J) {
            this->m_ent.getAttrType(*I, null, *J);
        }
    }
    return res;
}

inline bool EntityExerciser::checkProperties(const std::set<std::string> & prop_names)
{
    std::set<std::string>::const_iterator I = prop_names.begin();
    std::set<std::string>::const_iterator Iend = prop_names.end();
    for (; I != Iend; ++I) {
    }
    return true;
}

void EntityExerciser::addAllOperations(std::set<std::string> & ops)
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

inline void EntityExerciser::runOperations()
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

inline void EntityExerciser::runConversions()
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

inline void EntityExerciser::flushOperations(OpVector & ops)
{
    ops.clear();
}


#endif // TESTS_ENTITY_EXERCISER_H
