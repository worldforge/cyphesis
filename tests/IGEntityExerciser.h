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

// $Id: IGEntityExerciser.h,v 1.13 2007-07-31 19:14:32 alriddoch Exp $

#ifndef TESTS_IG_ENTITY_EXERCISER_H
#define TESTS_IG_ENTITY_EXERCISER_H

#include "EntityExerciser.h"
#include "TestPropertyManager.h"

#include <Atlas/Message/Element.h>

template <class EntityType>
class IGEntityExerciser : public EntityExerciser<EntityType> {
  public:
    explicit IGEntityExerciser(EntityType & e) :
                               EntityExerciser<EntityType>(e) {
        new TestPropertyManager;
        e.m_location.m_loc = new Entity("0", 0);
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
        if (!this->EntityExerciser<EntityType>::m_ent.getAttr(*I, null)) {
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
        if (this->EntityExerciser<EntityType>::m_ent.getProperty(*I) == NULL) {
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
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.LoginOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        Atlas::Objects::Entity::Anonymous login_arg;
        op->setArgs1(login_arg);
        EntityExerciser<EntityType>::m_ent.LoginOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Logout op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.LogoutOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Combine op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.CombineOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Create op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.CreateOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Delete op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.DeleteOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Divide op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.DivideOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Eat op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        op->setFrom(EntityExerciser<EntityType>::m_ent.getId());
        EntityExerciser<EntityType>::m_ent.EatOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Get op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.GetOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Imaginary op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.ImaginaryOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Info op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.InfoOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Move op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        Atlas::Objects::Root bad_arg;
        op->setArgs1(bad_arg);
        EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        Atlas::Objects::Entity::Anonymous move_arg;
        op->setArgs1(move_arg);
        EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        move_arg->setId(EntityExerciser<EntityType>::m_ent.getId());
        EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        move_arg->setLoc(EntityExerciser<EntityType>::m_ent.m_location.m_loc->getId());
        EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);

        // addToEntity(EntityExerciser<EntityType>::m_ent.m_location.pos(), move_arg->modifyPos());
        // EntityExerciser<EntityType>::m_ent.MoveOperation(op, ov);
        // EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Nourish op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.NourishOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Set op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.SetOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sight op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.SightOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sound op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.SoundOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Talk op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.TalkOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Touch op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.TouchOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Tick op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.TickOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Look op;
        op->setFrom(EntityExerciser<EntityType>::m_ent.getId());
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.LookOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Setup op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.SetupOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Appearance op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.AppearanceOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Disappearance op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.DisappearanceOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Use op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.UseOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Wield op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.WieldOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Error op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.ErrorOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::RootOperation op;
        EntityExerciser<EntityType>::dispatchOp(op);
        OpVector ov;
        EntityExerciser<EntityType>::m_ent.OtherOperation(op, ov);
        EntityExerciser<EntityType>::flushOperations(ov);
    }
}

#endif // TESTS_IG_ENTITY_EXERCISER_H
