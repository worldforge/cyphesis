// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BASE_ENTITY_H
#define COMMON_BASE_ENTITY_H

#include "types.h"
#include "operations.h"

#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>

/// \brief This is the base class from which all other entity like classes
/// inherit, both in game and out of game.
///
/// This class basically provides a means of delivering operations to
/// an object, and the structure required to process those operations.
/// It also provides a mechanism to create an Atlas representation of
/// the entity, and an operation enumeration and subscription mechanism
/// which ensures that operations are handled as efficiently as
/// possible.
/// It has an id member which is typically used to store it in a STL map or
/// dictionary as they are called elsewhere in this code.
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

    virtual void addToMessage(MapType &) const;

    virtual void operation(const RootOperation & op, OpVector &);

    virtual void LoginOperation(const Login & op, OpVector &);
    virtual void LogoutOperation(const Logout & op, OpVector &);
    virtual void ActionOperation(const Action & op, OpVector &);
    virtual void ChopOperation(const Chop & op, OpVector &);
    virtual void CombineOperation(const Combine & op, OpVector &);
    virtual void CreateOperation(const Create & op, OpVector &);
    virtual void CutOperation(const Cut & op, OpVector &);
    virtual void DeleteOperation(const Delete & op, OpVector &);
    virtual void DivideOperation(const Divide & op, OpVector &);
    virtual void EatOperation(const Eat & op, OpVector &);
    virtual void BurnOperation(const Burn & op, OpVector &);
    virtual void GetOperation(const Get & op, OpVector &);
    virtual void ImaginaryOperation(const Imaginary & op, OpVector &);
    virtual void InfoOperation(const Info & op, OpVector &);
    virtual void MoveOperation(const Move & op, OpVector &);
    virtual void NourishOperation(const Nourish & op, OpVector &);
    virtual void SetOperation(const Set & op, OpVector &);
    virtual void SightOperation(const Sight & op, OpVector &);
    virtual void SoundOperation(const Sound & op, OpVector &);
    virtual void TalkOperation(const Talk & op, OpVector &);
    virtual void TouchOperation(const Touch & op, OpVector &);
    virtual void TickOperation(const Tick & op, OpVector &);
    virtual void LookOperation(const Look & op, OpVector &);
    virtual void SetupOperation(const Setup & op, OpVector &);
    virtual void AppearanceOperation(const Appearance & op, OpVector &);
    virtual void DisappearanceOperation(const Disappearance & op, OpVector &);
    virtual void UseOperation(const Use & op, OpVector &);
    virtual void WieldOperation(const Wield & op, OpVector &);
    virtual void OtherOperation(const RootOperation & op, OpVector &);
    virtual void ErrorOperation(const RootOperation & op, OpVector &);

    OpNo opEnumerate(const RootOperation & op) const;
    OpNo opEnumerate(const RootOperation & op, const OpNoDict & d) const;
    void subscribe(const std::string & op);
    void callOperation(const RootOperation & op, OpVector &);
    void error(const RootOperation & op, const char * errstring, OpVector &,
                   const std::string & to = "") const;

    SigC::Signal0<void> destroyed;
};

#endif // COMMON_BASE_ENTITY_H
