// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "common/inheritance.h"
#include "common/types.h"

#include <cassert>

namespace Atlas {
    namespace Objects {
        namespace Operation {
            class Login;
            class Logout;
            class Action;
            class Combine;
            class Create;
            class Delete;
            class Divide;
            class Imaginary;
            class Move;
            class Set;
            class Get;
            class Perception;
            class Sight;
            class Sound;
            class Touch;
            class Talk;
            class Look;
            class Info;
            class Appearance;
            class Disappearance;
            class Error;
            class Setup;
            class Tick;
            class Eat;
            class Nourish;
            class Cut;
            class Chop;
            class Burn;
            class Use;
            class Wield;
            class Unseen;
            class Generic;
        }
    }
}

template <class EntityType>
class EntityExerciser {
  protected:
    EntityType & m_ent;
  public:
    explicit EntityExerciser(EntityType & e) : m_ent(e) { }

    virtual void dispatchOp(const Atlas::Objects::Operation::RootOperation&op) {
        OpVector ov1;
        m_ent.operation(op, ov1);
        flushOperations(ov1);
    }

    virtual void subscribeOp(const std::string & op) {
        m_ent.subscribe(op);
    }
    
    virtual void subscribeOperations(const std::set<std::string> & ops) {
        Inheritance & i = Inheritance::instance();
        std::set<std::string>::const_iterator I = ops.begin();
        std::set<std::string>::const_iterator Iend = ops.end();
        for (; I != Iend; ++I) {
            OpNo opNo = i.opEnumerate(*I);
            assert(opNo != OP_INVALID);
            subscribeOp(*I);
            opNo = OP_INVALID; // Supresses warning about unused variable.
        }
    }

    void addAllOperations(std::set<std::string> & ops);

    void runOperations();
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
        OpVector ov;
        m_ent.LoginOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Logout op;
        dispatchOp(op);
        OpVector ov;
        m_ent.LogoutOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Action op;
        dispatchOp(op);
        OpVector ov;
        m_ent.ActionOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Chop op;
        dispatchOp(op);
        OpVector ov;
        m_ent.ChopOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Combine op;
        dispatchOp(op);
        OpVector ov;
        m_ent.CombineOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Create op;
        dispatchOp(op);
        OpVector ov;
        m_ent.CreateOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Cut op;
        dispatchOp(op);
        OpVector ov;
        m_ent.CutOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Delete op;
        dispatchOp(op);
        OpVector ov;
        m_ent.DeleteOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Divide op;
        dispatchOp(op);
        OpVector ov;
        m_ent.DivideOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Eat op;
        dispatchOp(op);
        OpVector ov;
        m_ent.EatOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Burn op;
        dispatchOp(op);
        OpVector ov;
        m_ent.BurnOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Get op;
        dispatchOp(op);
        OpVector ov;
        m_ent.GetOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Imaginary op;
        dispatchOp(op);
        OpVector ov;
        m_ent.ImaginaryOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Info op;
        dispatchOp(op);
        OpVector ov;
        m_ent.InfoOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Move op;
        dispatchOp(op);
        OpVector ov;
        m_ent.MoveOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Nourish op;
        dispatchOp(op);
        OpVector ov;
        m_ent.NourishOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Set op;
        dispatchOp(op);
        OpVector ov;
        m_ent.SetOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sight op;
        dispatchOp(op);
        OpVector ov;
        m_ent.SightOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Sound op;
        dispatchOp(op);
        OpVector ov;
        m_ent.SoundOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Talk op;
        dispatchOp(op);
        OpVector ov;
        m_ent.TalkOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Touch op;
        dispatchOp(op);
        OpVector ov;
        m_ent.TouchOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Tick op;
        dispatchOp(op);
        OpVector ov;
        m_ent.TickOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Look op;
        dispatchOp(op);
        OpVector ov;
        m_ent.LookOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Setup op;
        dispatchOp(op);
        OpVector ov;
        m_ent.SetupOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Appearance op;
        dispatchOp(op);
        OpVector ov;
        m_ent.AppearanceOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Disappearance op;
        dispatchOp(op);
        OpVector ov;
        m_ent.DisappearanceOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Use op;
        dispatchOp(op);
        OpVector ov;
        m_ent.UseOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Wield op;
        dispatchOp(op);
        OpVector ov;
        m_ent.WieldOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::Error op;
        dispatchOp(op);
        OpVector ov;
        m_ent.ErrorOperation(op, ov);
        flushOperations(ov);
    }
    {
        Atlas::Objects::Operation::RootOperation op;
        dispatchOp(op);
        OpVector ov;
        m_ent.OtherOperation(op, ov);
        flushOperations(ov);
    }
}

template <class EntityType>
inline void EntityExerciser<EntityType>::flushOperations(OpVector & ops)
{
    OpVector::const_iterator Iend = ops.end();
    for (OpVector::const_iterator I = ops.begin(); I != Iend; ++I) {
        delete *I;
    }
}


#endif // TESTS_ENTITY_EXERCISER_H
