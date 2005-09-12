// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int NOURISH_NO;

class Nourish : public Generic
{
  public:
    Nourish() {
        (*this)->setType("nourish", NOURISH_NO);
    }
};

} } }

#endif // COMMON_NOURISH_H
