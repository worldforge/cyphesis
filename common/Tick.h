// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_TICK_H
#define COMMON_TICK_H

#include <Atlas/Objects/Operation/RootOperation.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to update an in-game entity
///
/// Used when the target entity needs its position information to be updated
/// regularly, and also when the entities mind needs to be updated regularly
/// so it can perform tasks.
class Tick : public RootOperation {
  protected:
    Tick(const char *, const char *);
  public:
    Tick();
    virtual ~Tick();
    static Tick Class();
};

} } }

#endif // COMMON_TICK_H
