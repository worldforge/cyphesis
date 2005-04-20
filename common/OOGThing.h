// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_OOG_THING_H
#define COMMON_OOG_THING_H

#include "BaseEntity.h"

/// \brief This is the base class from which all OOG entity classes inherit.
///
/// This class only overrides OtherOperation() ensuring that operations on
/// unknown types flag an error.
class OOGThing : public BaseEntity {
  protected:
    explicit OOGThing(const std::string & id);
  public:
    virtual ~OOGThing();
    virtual void OtherOperation(const Operation & op, OpVector &);
};

#endif // COMMON_OOG_THING_H
