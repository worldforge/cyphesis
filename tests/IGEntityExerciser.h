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

// $Id: IGEntityExerciser.h,v 1.26 2008-01-17 19:53:39 alriddoch Exp $

#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "EntityExerciser.h"

#include "TestPropertyManager.h"
#include "TestWorld.h"

#include "rulesets/Motion.h"

#include <Atlas/Message/Element.h>

template <class EntityType>
class IGEntityExerciser : public EntityExerciser<EntityType> {
  public:
    explicit IGEntityExerciser(EntityType & e) :
                               EntityExerciser<EntityType>(e) {
        new TestPropertyManager;
        if (e.getIntId() == 0) {
            new TestWorld(e);
            e.m_contains = new LocatedEntitySet;
        } else {
            e.m_location.m_loc = new Entity("0", 0);
            e.m_location.m_loc->m_contains = new LocatedEntitySet;
            e.m_location.m_loc->m_contains->insert(&e);
            new TestWorld(*dynamic_cast<Entity*>(e.m_location.m_loc));
        }
        BaseWorld::instance().addEntity(&e);
    }

    bool checkAttributes(const std::set<std::string> & attr_names);
    bool checkProperties(const std::set<std::string> & prop_names);

    void runOperations();
};

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkAttributes(const std::set<std::string> & attr_names)
{
    Atlas::Message::Element null;
    std::set<std::string>::const_iterator I = attr_names.begin();
    std::set<std::string>::const_iterator Iend = attr_names.end();
    for (; I != Iend; ++I) {
        if (!this->m_ent.getAttr(*I, null)) {
            std::cerr << "Entity does not have \"" << *I << "\" attribute."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

template <class EntityType>
inline bool IGEntityExerciser<EntityType>::checkProperties(const std::set<std::string> & prop_names)
{
    std::set<std::string>::const_iterator I = prop_names.begin();
    std::set<std::string>::const_iterator Iend = prop_names.end();
    for (; I != Iend; ++I) {
        if (this->m_ent.getProperty(*I) == NULL) {
            std::cerr << "Entity does not have \"" << *I << "\" property."
                      << std::endl << std::flush;
            return false;
        }
    }
    return true;
}

template <class EntityType>
inline void IGEntityExerciser<EntityType>::runOperations()
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
        Atlas::Objects::Operation::Combine op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.CombineOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Create op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.CreateOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent.CreateOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous create_arg;
        op->setArgs1(create_arg);
        this->m_ent.CreateOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        create_arg->setParents(std::list<std::string>(1, ""));
        this->m_ent.CreateOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        create_arg->setParents(std::list<std::string>(1, "thing"));
        this->m_ent.CreateOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Delete op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.DeleteOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Divide op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.DivideOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Eat op;
        this->dispatchOp(op);
        OpVector ov;
        op->setFrom(this->m_ent.getId());
        this->m_ent.EatOperation(op, ov);
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
        this->m_ent.ImaginaryOperation(op, ov);
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
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous move_arg;
        op->setArgs1(move_arg);
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setId(this->m_ent.getId());
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setLoc("242");
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setLoc(this->m_ent.getId());
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        if (this->m_ent.m_location.m_loc != 0) {
            move_arg->setLoc(this->m_ent.m_location.m_loc->getId());
            this->m_ent.MoveOperation(op, ov);
            if (!ov.empty()) {
                assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
            }
            this->flushOperations(ov);
        }

        move_arg->setLoc(this->m_ent.getId());
        addToEntity(this->m_ent.m_location.pos(), move_arg->modifyPos());
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->setAttr("mode", 1);
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        move_arg->removeAttr("mode");
        this->m_ent.MoveOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Nourish op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.NourishOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->dispatchOp(op);
        this->m_ent.NourishOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous nourish_arg;
        op->setArgs1(nourish_arg);
        this->dispatchOp(op);
        this->m_ent.NourishOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        nourish_arg->setAttr("mass", 23);
        this->dispatchOp(op);
        this->m_ent.NourishOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Set op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::ERROR_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Root empty_arg;
        op->setArgs1(empty_arg);
        this->m_ent.SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous set_arg;
        op->setArgs1(set_arg);
        this->m_ent.SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);

        op->setAttr("status", -1);
        this->m_ent.SetOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
            if (ov.size() > 1) {
                assert(ov[1]->getClassNo() == Atlas::Objects::Operation::DELETE_NO);
            }
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sight op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.SightOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sound op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.SoundOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Talk op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.TalkOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SOUND_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Touch op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.TouchOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Tick op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.TickOperation(op, ov);
        if (!ov.empty()) {
            // assert(ov.front()->getClassNo() == Atlas::Objects::Operation::TICK_NO);
            // FIXME We expect TICK, or some others.
        }
        this->flushOperations(ov);

        Atlas::Objects::Root tick_arg;
        op->setArgs1(tick_arg);
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("move");
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", "non-number");
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", -1);
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setAttr("serialno", 0);
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->removeAttr("serialno");
        tick_arg->setName("task");
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("mind");
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);

        tick_arg->setName("non-existant-subsystem");
        this->m_ent.TickOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Look op;
        op->setFrom(this->m_ent.getId());
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.LookOperation(op, ov);
        if (!ov.empty()) {
            assert(ov.front()->getClassNo() == Atlas::Objects::Operation::SIGHT_NO);
        }
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Setup op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.SetupOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous setup_arg;
        op->setArgs1(setup_arg);
        this->m_ent.SetupOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Appearance op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.AppearanceOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Disappearance op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.DisappearanceOperation(op, ov);
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
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Entity::Anonymous update_arg;
        op->setArgs1(update_arg);
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        update_arg->setName("foo");
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        update_arg->setAttr("status", -1);
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent.m_location.m_velocity = Vector3D();
        if (this->m_ent.motion() != 0) {
            op->setRefno(this->m_ent.motion()->serialno());
        }
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);

        this->m_ent.m_location.m_velocity = Vector3D(1,0,0);
        this->m_ent.UpdateOperation(op, ov);
        this->flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Wield op;
        this->dispatchOp(op);
        OpVector ov;
        this->m_ent.WieldOperation(op, ov);
        this->flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        this->m_ent.WieldOperation(op, ov);
        this->flushOperations(ov);

        bad_arg->setId("23");
        this->m_ent.WieldOperation(op, ov);
        this->flushOperations(ov);

        bad_arg->setId("1");
        this->m_ent.WieldOperation(op, ov);
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

#endif // TESTS_IG_ENTITY_EXERCISER_H
