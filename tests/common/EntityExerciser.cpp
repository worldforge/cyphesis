// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "EntityExerciser.h"

#include "common/OperationRouter.h"
#include "common/TypeNode.h"
#include "common/Property.h"

#include "common/operations/Connect.h"
#include "common/operations/Monitor.h"
#include "common/operations/Setup.h"
#include "common/operations/Tick.h"
#include "common/operations/Update.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <set>

#include <cassert>

PropertyManager* LocatedEntityTest::propertyManager = nullptr;

void LocatedEntityTest::externalOperation(const Operation & op, Link &)
{
}

void LocatedEntityTest::operation(const Operation &, OpVector &)
{
}


EntityExerciser::EntityExerciser(Ref<LocatedEntity> e) : m_ent(e)
{
    if (e->getIntId() == 0) {
        e->makeContainer();
        assert(e->m_contains != 0);
    } else {
        m_parentEntity = Ref<LocatedEntity>(new LocatedEntityTest(0));
        e->m_parent = m_parentEntity.get();
        m_parentEntity->makeContainer();
        assert(m_parentEntity->m_contains != 0);
        m_parentEntity->m_contains->insert(e);
    }
    if (e->getType() == 0) {
        test_type = std::make_unique<TypeNode>("test_type");
        test_type->injectProperty("test_default", std::make_unique<SoftProperty>());
        e->setType(test_type.get());
    }
    attr_types.insert(Atlas::Message::Element::TYPE_INT);
    attr_types.insert(Atlas::Message::Element::TYPE_FLOAT);
    attr_types.insert(Atlas::Message::Element::TYPE_STRING);
    attr_types.insert(Atlas::Message::Element::TYPE_MAP);
    attr_types.insert(Atlas::Message::Element::TYPE_LIST);
}

EntityExerciser::~EntityExerciser()
{
//    if (m_ent->getIntId() != 0) {
//        m_ent->m_parent = nullptr;
//    }
    if (test_type) {
        m_ent->setType(nullptr);
    }
}

void EntityExerciser::dispatchOp(const Atlas::Objects::Operation::RootOperation&op)
{
    OpVector ov1;
    m_ent->operation(op, ov1);
    flushOperations(ov1);
}


bool EntityExerciser::checkAttributes(const std::set<std::string> & attr_names)
{
    Atlas::Message::Element null;
    bool res = true;
    auto I = attr_names.begin();
    auto Iend = attr_names.end();
    for (; I != Iend; ++I) {
        if (this->m_ent->getAttr(*I, null) != 0) {
            std::cerr << "Entity does not have \"" << *I << "\" attribute."
                      << std::endl << std::flush;
            res = false;
        }
        if (this->m_ent->getProperty(*I) == nullptr) {
            std::cerr << "Entity does not have \"" << *I << "\" property."
                      << std::endl << std::flush;
            res = false;
        }
        auto J = attr_types.begin();
        auto Jend = attr_types.end();
        for (; J != Jend; ++J) {
            this->m_ent->getAttrType(*I, null, *J);
        }
    }
    return res;
}

bool EntityExerciser::checkProperties(const std::set<std::string> & prop_names)
{
    auto I = prop_names.begin();
    auto Iend = prop_names.end();
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
    ops.insert("create");
    ops.insert("cut");
    ops.insert("delete");
    ops.insert("consume");
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

void EntityExerciser::runOperations()
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
        Atlas::Objects::Operation::Create op;
        dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Delete op;
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
        op->setFrom(m_ent->getId());
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

void EntityExerciser::runConversions()
{
    {
        Atlas::Message::MapType data;

        assert(data.empty());
        m_ent->addToMessage(data);
        assert(!data.empty());
    }

    {
        Atlas::Objects::Entity::Anonymous data;

        assert(data->isDefaultId());
        m_ent->addToEntity(data);
        assert(!data->isDefaultId());
    }
}

void EntityExerciser::flushOperations(OpVector & ops)
{
    ops.clear();
}

// stubs

TypeNode::PropertiesUpdate TypeNode::injectProperty(const std::string& name,
                              std::unique_ptr<PropertyBase> p)
{
    m_defaults[name] = std::move(p);
    return {};
}
