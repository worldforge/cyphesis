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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "IGEntityExerciser.h"

#include "TestPropertyManager.h"
#include "TestWorld.h"

#include "rules/simulation/Entity.h"

#include "common/TypeNode.h"

#include "common/operations/Connect.h"
#include "common/operations/Monitor.h"
#include "common/operations/Setup.h"
#include "common/operations/Tick.h"
#include "common/operations/Update.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

IGEntityExerciser::IGEntityExerciser(const Ref<Entity>& e) :
                           EntityExerciser(e), m_ent(e) {
    if (e->getIntId() == 0) {
        m_testWorld = std::make_unique<TestWorld>(e);
    } else {
        assert(e->m_parent != nullptr);
        m_testWorld = std::make_unique<TestWorld>(e->m_parent);
        m_testWorld->addEntity(e, e->m_parent);
    }
}

IGEntityExerciser::~IGEntityExerciser() {
    m_testWorld->shutdown();
};

bool IGEntityExerciser::checkProperties(const std::set<std::string> & prop_names)
{
    auto I = prop_names.begin();
    auto Iend = prop_names.end();
    for (; I != Iend; ++I) {
        if (this->m_ent->getProperty(*I) == nullptr) {
            std::cerr << "Entity does not have \"" << *I << "\" property."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

void IGEntityExerciser::runOperations()
{
    {
        Atlas::Objects::Operation::Login op;
        this->dispatchOp(op);

        Atlas::Objects::Entity::Anonymous login_arg;
        op->setArgs1(login_arg);
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Logout op;
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Create op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous create_arg;
        op->setArgs1(create_arg);
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        create_arg->setParent("");
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        create_arg->setParent("thing");
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Delete op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->DeleteOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Get op;
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Imaginary op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->ImaginaryOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Info op;
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Move op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous move_arg;
        op->setArgs1(move_arg);
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setLoc("242");
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setLoc(this->m_ent->getId());
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        if (this->m_ent->m_parent != nullptr) {
            move_arg->setLoc(this->m_ent->m_parent->getId());
            this->m_ent->MoveOperation(op, ov);
            if (!ov.empty()) {
                assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
            }
            this->flushOperations(ov);
        }

        move_arg->setLoc(this->m_ent->getId());
        addToEntity(this->m_ent->requirePropertyClassFixed<PositionProperty>().data(), move_arg->modifyPos());
        this->m_ent->MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        if (this->m_ent->m_parent != nullptr) {
            move_arg->setLoc(this->m_ent->m_parent->getId());
        }
        std::vector<double> pos(3, 0);
        move_arg->setPos(pos);
        this->m_ent->MoveOperation(op, ov);

        move_arg->setAttr("mode", 1);
        this->m_ent->MoveOperation(op, ov);
        this->flushOperations(ov);

        move_arg->setAttr("mode", "standing");
        this->m_ent->MoveOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent->MoveOperation(op, ov);
        this->flushOperations(ov);

        move_arg->removeAttr("mode");
        this->m_ent->MoveOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent->addFlags(entity_perceptive);
        this->m_ent->MoveOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Set op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root empty_arg;
        op->setArgs1(empty_arg);
        this->m_ent->SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::UPDATE_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous set_arg;
        op->setArgs1(set_arg);
        this->m_ent->SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::UPDATE_NO);
        }
        this->flushOperations(ov);

        op->setAttr("status", -1);
        this->m_ent->SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::UPDATE_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sight op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sound op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Talk op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->TalkOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SOUND_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Touch op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Tick op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        if (!ov.empty()) {
            // assert(ov.front()->getClassNo() == Atlas::Objects::Operation::TICK_NO);
            // FIXME We expect TICK, or some others.
        }
        this->flushOperations(ov);

        Atlas::Objects::Root tick_arg;
        op->setArgs1(tick_arg);
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("move");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", "non-number");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", -1);
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", 0);
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->removeAttr("serialno");
        tick_arg->setName("task");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("mind");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("non-existent-subsystem");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Look op;
        op->setFrom(this->m_ent->getId());
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->LookOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Appearance op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Disappearance op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Use op;
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::Update op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous update_arg;
        op->setArgs1(update_arg);
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        update_arg->setName("foo");
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        update_arg->setAttr("status", -1);
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent->requirePropertyClassFixed<VelocityProperty>().data() = Vector3D();
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent->requirePropertyClassFixed<PositionProperty>().data() = Point3D(0,0,0);
        this->m_ent->requirePropertyClassFixed<VelocityProperty>().data() = Vector3D(1,0,0);
        this->m_ent->UpdateOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Wield op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        bad_arg->setId("23");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);

        bad_arg->setId("1");
        this->m_ent->operation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Error op;
        this->dispatchOp(op);
    }
    {
        Atlas::Objects::Operation::RootOperation op;
        this->dispatchOp(op);
    }
}
