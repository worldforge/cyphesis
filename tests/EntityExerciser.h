// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "common/operations.h"
#include "common/inheritance.h"
#include "common/types.h"

#include <cassert>

template <class EntityType>
class EntityExerciser {
  protected:
    EntityType & m_ent;
  public:
    explicit EntityExerciser(EntityType & e) : m_ent(e) { }

    virtual void dispatchOp(const RootOperation & op) {
        OpVector ov1 = m_ent.message(op);
        OpVector ov2 = m_ent.operation(op);
        OpVector ov3 = m_ent.externalMessage(op);
        OpVector ov4 = m_ent.externalOperation(op);
        flushOperations(ov1);
        flushOperations(ov2);
        flushOperations(ov3);
        flushOperations(ov4);
    }

    virtual void subscribeOp(const std::string & op) {
        m_ent.subscribe(op);
    }
    
    virtual void subscribeOperations(const std::set<std::string> & ops) {
        Inheritance & i = Inheritance::instance();
        std::set<std::string>::const_iterator I = ops.begin();
        for(; I != ops.end(); ++I) {
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
    ops.insert("error");
}

template <class EntityType>
inline void EntityExerciser<EntityType>::runOperations()
{
    {
        Login op;
        dispatchOp(op);
        OpVector ov = m_ent.LoginOperation(op);
        flushOperations(ov);
    }
    {
        Logout op;
        dispatchOp(op);
        OpVector ov = m_ent.LogoutOperation(op);
        flushOperations(ov);
    }
    {
        Action op;
        dispatchOp(op);
        OpVector ov = m_ent.ActionOperation(op);
        flushOperations(ov);
    }
    {
        Chop op;
        dispatchOp(op);
        OpVector ov = m_ent.ChopOperation(op);
        flushOperations(ov);
    }
    {
        Combine op;
        dispatchOp(op);
        OpVector ov = m_ent.CombineOperation(op);
        flushOperations(ov);
    }
    {
        Create op;
        dispatchOp(op);
        OpVector ov = m_ent.CreateOperation(op);
        flushOperations(ov);
    }
    {
        Cut op;
        dispatchOp(op);
        OpVector ov = m_ent.CutOperation(op);
        flushOperations(ov);
    }
    {
        Delete op;
        dispatchOp(op);
        OpVector ov = m_ent.DeleteOperation(op);
        flushOperations(ov);
    }
    {
        Divide op;
        dispatchOp(op);
        OpVector ov = m_ent.DivideOperation(op);
        flushOperations(ov);
    }
    {
        Eat op;
        dispatchOp(op);
        OpVector ov = m_ent.EatOperation(op);
        flushOperations(ov);
    }
    {
        Burn op;
        dispatchOp(op);
        OpVector ov = m_ent.BurnOperation(op);
        flushOperations(ov);
    }
    {
        Get op;
        dispatchOp(op);
        OpVector ov = m_ent.GetOperation(op);
        flushOperations(ov);
    }
    {
        Imaginary op;
        dispatchOp(op);
        OpVector ov = m_ent.ImaginaryOperation(op);
        flushOperations(ov);
    }
    {
        Info op;
        dispatchOp(op);
        OpVector ov = m_ent.InfoOperation(op);
        flushOperations(ov);
    }
    {
        Move op;
        dispatchOp(op);
        OpVector ov = m_ent.MoveOperation(op);
        flushOperations(ov);
    }
    {
        Nourish op;
        dispatchOp(op);
        OpVector ov = m_ent.NourishOperation(op);
        flushOperations(ov);
    }
    {
        Set op;
        dispatchOp(op);
        OpVector ov = m_ent.SetOperation(op);
        flushOperations(ov);
    }
    {
        Sight op;
        dispatchOp(op);
        OpVector ov = m_ent.SightOperation(op);
        flushOperations(ov);
    }
    {
        Sound op;
        dispatchOp(op);
        OpVector ov = m_ent.SoundOperation(op);
        flushOperations(ov);
    }
    {
        Talk op;
        dispatchOp(op);
        OpVector ov = m_ent.TalkOperation(op);
        flushOperations(ov);
    }
    {
        Touch op;
        dispatchOp(op);
        OpVector ov = m_ent.TouchOperation(op);
        flushOperations(ov);
    }
    {
        Tick op;
        dispatchOp(op);
        OpVector ov = m_ent.TickOperation(op);
        flushOperations(ov);
    }
    {
        Look op;
        dispatchOp(op);
        OpVector ov = m_ent.LookOperation(op);
        flushOperations(ov);
    }
    {
        Setup op;
        dispatchOp(op);
        OpVector ov = m_ent.SetupOperation(op);
        flushOperations(ov);
    }
    {
        Appearance op;
        dispatchOp(op);
        OpVector ov = m_ent.AppearanceOperation(op);
        flushOperations(ov);
    }
    {
        Disappearance op;
        dispatchOp(op);
        OpVector ov = m_ent.DisappearanceOperation(op);
        flushOperations(ov);
    }
    {
        Error op;
        dispatchOp(op);
        OpVector ov = m_ent.ErrorOperation(op);
        flushOperations(ov);
    }
    {
        RootOperation op;
        dispatchOp(op);
        OpVector ov = m_ent.OtherOperation(op);
        flushOperations(ov);
    }
}

template <class EntityType>
inline void EntityExerciser<EntityType>::flushOperations(OpVector & ops)
{
    for(OpVector::const_iterator I = ops.begin(); I != ops.end(); ++I) {
        delete *I;
    }
}


#endif // TESTS_ENTITY_EXERCISER_H
