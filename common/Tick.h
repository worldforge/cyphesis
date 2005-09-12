// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_TICK_H
#define COMMON_TICK_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int TICK_NO;

class Tick : public Generic
{
  public:
    Tick() {
        (*this)->setType("tick", TICK_NO);
    }
};

} } }

#endif // COMMON_TICK_H
