// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_ENTITY_H
#define COMMON_BASE_ENTITY_H

#include "types.h"
#include "operations.h"

#include <Atlas/Message/Element.h>

#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>

// This is the base class from which all other entity like classes inherit,
// both in game and out of game.
// This class basically provides a means of delivering operations to
// an object, and the structure required to process those operations.
// It has an id which is typically used to store it in a map or
// dictionary as they are called elsewhere in this code.

class BaseEntity : virtual public SigC::Object {
  private:
    const std::string m_id;		// String id

    // Private and un-implemented, to make sure slicing is impossible
    BaseEntity(const BaseEntity &);
    const BaseEntity & operator=(const BaseEntity &);
  protected:
    OpNoDict opLookup;

    explicit BaseEntity(const std::string & id);

    void subscribe(const std::string& op, OpNo no) {
        opLookup[op] = no;
    }
  public:
    virtual ~BaseEntity();

    const std::string & getId() const {
        return m_id;
    }

    virtual void addToObject(Atlas::Message::Element::MapType &) const;

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
    virtual OpVector BurnOperation(const Burn & op);
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
    virtual OpVector SetupOperation(const Setup & op);
    virtual OpVector AppearanceOperation(const Appearance & op);
    virtual OpVector DisappearanceOperation(const Disappearance & op);
    virtual OpVector UseOperation(const Use & op);
    virtual OpVector WieldOperation(const Wield & op);
    virtual OpVector OtherOperation(const RootOperation & op);
    virtual OpVector ErrorOperation(const RootOperation & op);

    OpNo opEnumerate(const RootOperation & op) const;
    OpNo opEnumerate(const RootOperation & op, const OpNoDict & d) const;
    void subscribe(const std::string & op);
    OpVector callOperation(const RootOperation & op);
    OpVector error(const RootOperation & op, const char * errstring,
                   const std::string & to = "") const;

    SigC::Signal0<void> destroyed;
};

#endif // COMMON_BASE_ENTITY_H
