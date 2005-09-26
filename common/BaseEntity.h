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
    explicit BaseEntity(const std::string & id);
  public:
    virtual ~BaseEntity();

    const std::string & getId() const {
        return m_id;
    }

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    virtual void operation(const Operation &, OpVector &);

    virtual void ActionOperation(const Operation &, OpVector &);
    virtual void AddOperation(const Operation &, OpVector &);
    virtual void AppearanceOperation(const Operation &, OpVector &);
    virtual void AttackOperation(const Operation &, OpVector &);
    virtual void BurnOperation(const Operation &, OpVector &);
    virtual void ChopOperation(const Operation &, OpVector &);
    virtual void CombineOperation(const Operation &, OpVector &);
    virtual void CreateOperation(const Operation &, OpVector &);
    virtual void CutOperation(const Operation &, OpVector &);
    virtual void DeleteOperation(const Operation &, OpVector &);
    virtual void DisappearanceOperation(const Operation &, OpVector &);
    virtual void DivideOperation(const Operation &, OpVector &);
    virtual void EatOperation(const Operation &, OpVector &);
    virtual void GetOperation(const Operation &, OpVector &);
    virtual void ImaginaryOperation(const Operation &, OpVector &);
    virtual void InfoOperation(const Operation &, OpVector &);
    virtual void LoginOperation(const Operation &, OpVector &);
    virtual void LogoutOperation(const Operation &, OpVector &);
    virtual void LookOperation(const Operation &, OpVector &);
    virtual void MoveOperation(const Operation &, OpVector &);
    virtual void NourishOperation(const Operation &, OpVector &);
    virtual void SetOperation(const Operation &, OpVector &);
    virtual void SetupOperation(const Operation &, OpVector &);
    virtual void SightOperation(const Operation &, OpVector &);
    virtual void SoundOperation(const Operation &, OpVector &);
    virtual void TalkOperation(const Operation &, OpVector &);
    virtual void TickOperation(const Operation &, OpVector &);
    virtual void TouchOperation(const Operation &, OpVector &);
    virtual void UpdateOperation(const Operation &, OpVector &);
    virtual void UseOperation(const Operation &, OpVector &);
    virtual void WieldOperation(const Operation &, OpVector &);

    virtual void OtherOperation(const Operation &, OpVector &);
    virtual void ErrorOperation(const Operation &, OpVector &);

    OpNo opEnumerate(const Operation &) const;
    void callOperation(const Operation &, OpVector &);
    void error(const Operation &, const char * errstring, OpVector &,
                   const std::string & to = "") const;

    SigC::Signal0<void> destroyed;
};

#endif // COMMON_BASE_ENTITY_H
