// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_BASE_ENTITY_H
#define COMMON_BASE_ENTITY_H

#include "types.h"

#include <Atlas/Message/Element.h>

#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>

using Atlas::Objects::Operation::RootOperation;

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
    /// String id.
    const std::string m_id;

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

    virtual void addToMessage(Atlas::Message::MapType &) const;

    virtual void operation(const RootOperation &, OpVector &);

    virtual void LoginOperation(const RootOperation &, OpVector &);
    virtual void LogoutOperation(const RootOperation &, OpVector &);
    virtual void ActionOperation(const RootOperation &, OpVector &);
    virtual void ChopOperation(const RootOperation &, OpVector &);
    virtual void CombineOperation(const RootOperation &, OpVector &);
    virtual void CreateOperation(const RootOperation &, OpVector &);
    virtual void CutOperation(const RootOperation &, OpVector &);
    virtual void DeleteOperation(const RootOperation &, OpVector &);
    virtual void DivideOperation(const RootOperation &, OpVector &);
    virtual void EatOperation(const RootOperation &, OpVector &);
    virtual void BurnOperation(const RootOperation &, OpVector &);
    virtual void GetOperation(const RootOperation &, OpVector &);
    virtual void ImaginaryOperation(const RootOperation &, OpVector &);
    virtual void InfoOperation(const RootOperation &, OpVector &);
    virtual void MoveOperation(const RootOperation &, OpVector &);
    virtual void NourishOperation(const RootOperation &, OpVector &);
    virtual void SetOperation(const RootOperation &, OpVector &);
    virtual void SightOperation(const RootOperation &, OpVector &);
    virtual void SoundOperation(const RootOperation &, OpVector &);
    virtual void TalkOperation(const RootOperation &, OpVector &);
    virtual void TouchOperation(const RootOperation &, OpVector &);
    virtual void TickOperation(const RootOperation &, OpVector &);
    virtual void LookOperation(const RootOperation &, OpVector &);
    virtual void SetupOperation(const RootOperation &, OpVector &);
    virtual void AppearanceOperation(const RootOperation &, OpVector &);
    virtual void DisappearanceOperation(const RootOperation &, OpVector &);
    virtual void UseOperation(const RootOperation &, OpVector &);
    virtual void WieldOperation(const RootOperation &, OpVector &);
    virtual void OtherOperation(const RootOperation &, OpVector &);
    virtual void ErrorOperation(const RootOperation &, OpVector &);

    OpNo opEnumerate(const RootOperation &) const;
    OpNo opEnumerate(const RootOperation &, const OpNoDict & d) const;
    void subscribe(const std::string &);
    void callOperation(const RootOperation &, OpVector &);
    void error(const RootOperation &, const char * errstring, OpVector &,
                   const std::string & to = "") const;

    SigC::Signal0<void> destroyed;
};

#endif // COMMON_BASE_ENTITY_H
