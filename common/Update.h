// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_UPDATE_H
#define COMMON_UPDATE_H

#include "Tick.h"

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to update the position of an in-game entity
///
/// This replaces Tick for movement updates, and Tick is now only used for
/// handling minds and other generic regular updates.
class Update : public Tick {
  protected:
    Update(const char *, const char *);
  public:
    Update();
    virtual ~Update();
    static Update Class();
};

} } }

#endif // COMMON_UPDATE_H
