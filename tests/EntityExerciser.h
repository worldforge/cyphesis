// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "common/operations.h"
#include "common/types.h"

template <class EntityType>
class EntityExerciser {
  private:
    EntityType & m_ent;
  public:
    explicit EntityExerciser(EntityType & e) : m_ent(e) { }

    virtual void dispatchOp(const RootOperation & op) {
        OpVector ov1 = m_ent.message(op);
        OpVector ov2 = m_ent.operation(op);
        OpVector ov3 = m_ent.externalMessage(op);
        OpVector ov4 = m_ent.externalOperation(op);
    }

    void runOperations();
};

template <class EntityType>
inline void EntityExerciser<EntityType>::runOperations()
{
    {
        Login op(Login::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.LoginOperation(op);
    }
    {
        Logout op(Logout::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.LogoutOperation(op);
    }
    {
        Action op(Action::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.ActionOperation(op);
    }
    {
        Chop op(Chop::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.ChopOperation(op);
    }
    {
        Combine op(Combine::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.CombineOperation(op);
    }
    {
        Create op(Create::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.CreateOperation(op);
    }
    {
        Cut op(Cut::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.CutOperation(op);
    }
    {
        Delete op(Delete::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.DeleteOperation(op);
    }
    {
        Divide op(Divide::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.DivideOperation(op);
    }
    {
        Eat op(Eat::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.EatOperation(op);
    }
    {
        Burn op(Burn::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.BurnOperation(op);
    }
    {
        Get op(Get::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.GetOperation(op);
    }
    {
        Imaginary op(Imaginary::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.ImaginaryOperation(op);
    }
    {
        Info op(Info::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.InfoOperation(op);
    }
    {
        Move op(Move::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.MoveOperation(op);
    }
    {
        Nourish op(Nourish::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.NourishOperation(op);
    }
    {
        Set op(Set::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.SetOperation(op);
    }
    {
        Sight op(Sight::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.SightOperation(op);
    }
    {
        Sound op(Sound::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.SoundOperation(op);
    }
    {
        Talk op(Talk::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.TalkOperation(op);
    }
    {
        Touch op(Touch::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.TouchOperation(op);
    }
    {
        Tick op(Tick::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.TickOperation(op);
    }
    {
        Look op(Look::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.LookOperation(op);
    }
    {
        Setup op(Setup::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.SetupOperation(op);
    }
    {
        Appearance op(Appearance::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.AppearanceOperation(op);
    }
    {
        Disappearance op(Disappearance::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.DisappearanceOperation(op);
    }
    {
        Error op(Error::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.ErrorOperation(op);
    }
    {
        RootOperation op(RootOperation::Instantiate());
        dispatchOp(op);
        OpVector ov = m_ent.OtherOperation(op);
    }
}

#endif // TESTS_ENTITY_EXERCISER_H
