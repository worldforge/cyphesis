// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_ENTITY_H
#define COMMON_BASE_ENTITY_H

#include <common/types.h>
#include <common/operations.h>

// This is the base class from which all other entity like classes inherit,
// both in game and out of game.
// This class basically provides a means of delivering operations to
// an object, and the structure required to process those operations.
// It has an id which is typically used to store it in a map or
// dictionary as they are called elsewhere in this code.

class BaseEntity {
  private:
    std::string m_id;		// String id

    // Private and un-implemented, to make sure slicing is impossible
    BaseEntity(const BaseEntity &);
    const BaseEntity & operator=(const BaseEntity &);
  public:
    BaseEntity();
    virtual ~BaseEntity();

    const std::string & getId() const {
        return m_id;
    }

    void setId(const std::string & i) {
        m_id = i;
    }

    Atlas::Message::Object asObject() const;
    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual OpVector message(const RootOperation & op);
    virtual OpVector operation(const RootOperation & op);
    virtual OpVector externalMessage(const RootOperation & op);
    virtual OpVector externalOperation(const RootOperation & op);

    virtual OpVector LoginOperation(const Login & op);
    virtual OpVector LogoutOperation(const Logout & op);
    virtual OpVector ActionOperation(const Action & op);
    virtual OpVector ChopOperation(const Chop & op);
    virtual OpVector CombineOperation(const Combine & op);
    virtual OpVector CreateOperation(const Create & op);
    virtual OpVector CutOperation(const Cut & op);
    virtual OpVector DeleteOperation(const Delete & op);
    virtual OpVector DivideOperation(const Divide & op);
    virtual OpVector EatOperation(const Eat & op);
    virtual OpVector FireOperation(const Fire & op);
    virtual OpVector GetOperation(const Get & op);
    virtual OpVector ImaginaryOperation(const Imaginary & op);
    virtual OpVector InfoOperation(const Info & op);
    virtual OpVector MoveOperation(const Move & op);
    virtual OpVector NourishOperation(const Nourish & op);
    virtual OpVector SetOperation(const Set & op);
    virtual OpVector SightOperation(const Sight & op);
    virtual OpVector SoundOperation(const Sound & op);
    virtual OpVector TalkOperation(const Talk & op);
    virtual OpVector TouchOperation(const Touch & op);
    virtual OpVector TickOperation(const Tick & op);
    virtual OpVector LookOperation(const Look & op);
    virtual OpVector LoadOperation(const Load & op);
    virtual OpVector SaveOperation(const Save & op);
    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector AppearanceOperation(const Appearance & op);
    virtual OpVector DisappearanceOperation(const Disappearance & op);
    virtual OpVector OtherOperation(const RootOperation & op);
    virtual OpVector ErrorOperation(const RootOperation & op);

    void setRefno(const OpVector & ret, const RootOperation & ref_op) const;
    OpNo opEnumerate(const RootOperation & op) const;
    OpVector callOperation(const RootOperation & op);
    OpVector error(const RootOperation & op, const char * errstring) const;

    void setRefnoOp(RootOperation * op, const RootOperation & ref_op) const {
        op->SetRefno(ref_op.GetSerialno());
    }

};

#endif // COMMON_BASE_ENTITY_H
