// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_FIRE_H
#define COMMON_FIRE_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

class Fire : public Action {
  public:
    Fire();
    virtual ~Fire();
    static Fire Instantiate();
};

} } }

#endif // COMMON_FIRE_H
