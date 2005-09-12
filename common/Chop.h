// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CHOP_H
#define COMMON_CHOP_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int CHOP_NO;

class Chop : public Generic
{
  public:
    Chop() {
        (*this)->setType("chop", CHOP_NO);
    }
};

} } }

#endif // COMMON_CHOP_H
