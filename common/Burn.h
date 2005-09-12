// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_BURN_H
#define COMMON_BURN_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int BURN_NO;

class Burn : public Generic
{
  public:
    Burn() {
        (*this)->setType("burn", BURN_NO);
    }
};

} } }

#endif // COMMON_BURN_H
