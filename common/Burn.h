// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_BURN_H
#define COMMON_BURN_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

class Burn : public Action {
  public:
    Burn();
    virtual ~Burn();
    static Burn Instantiate();
};

} } }

#endif // COMMON_BURN_H
