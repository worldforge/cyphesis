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
  protected:
    bool game;		// true if in game object
  public:
    BaseEntity();
    virtual ~BaseEntity();

    const bool inGame() const { return game; }

    const std::string & getId() const {
        return m_id;
    }

    void setId(const string & i) {
        m_id = i;
    }

    virtual void destroy();

    Atlas::Message::Object asObject() const;
    virtual void addToObject(Atlas::Message::Object::MapType &) const;

    virtual oplist message(const RootOperation & op);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalMessage(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);

    virtual oplist LoginOperation(const Login & op);
    virtual oplist LogoutOperation(const Logout & op);
    virtual oplist ActionOperation(const Action & op);
    virtual oplist ChopOperation(const Chop & op);
    virtual oplist CombineOperation(const Combine & op);
    virtual oplist CreateOperation(const Create & op);
    virtual oplist CutOperation(const Cut & op);
    virtual oplist DeleteOperation(const Delete & op);
    virtual oplist DivideOperation(const Divide & op);
    virtual oplist EatOperation(const Eat & op);
    virtual oplist FireOperation(const Fire & op);
    virtual oplist GetOperation(const Get & op);
    virtual oplist ImaginaryOperation(const Imaginary & op);
    virtual oplist InfoOperation(const Info & op);
    virtual oplist MoveOperation(const Move & op);
    virtual oplist NourishOperation(const Nourish & op);
    virtual oplist SetOperation(const Set & op);
    virtual oplist SightOperation(const Sight & op);
    virtual oplist SoundOperation(const Sound & op);
    virtual oplist TalkOperation(const Talk & op);
    virtual oplist TouchOperation(const Touch & op);
    virtual oplist TickOperation(const Tick & op);
    virtual oplist LookOperation(const Look & op);
    virtual oplist LoadOperation(const Load & op);
    virtual oplist SaveOperation(const Save & op);
    virtual oplist SetupOperation(const Setup & op);
    virtual oplist AppearanceOperation(const Appearance & op);
    virtual oplist DisappearanceOperation(const Disappearance & op);
    virtual oplist OtherOperation(const RootOperation & op);
    virtual oplist ErrorOperation(const RootOperation & op);

    void setRefno(const oplist & ret, const RootOperation & ref_op) const;
    op_no_t opEnumerate(const RootOperation & op) const;
    oplist callOperation(const RootOperation & op);
    oplist error(const RootOperation & op, const char * errstring) const;

    void setRefnoOp(RootOperation * op, const RootOperation & ref_op) const {
        op->SetRefno(ref_op.GetSerialno());
    }

};

#endif // COMMON_BASE_ENTITY_H
