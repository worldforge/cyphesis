// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_DIG_H
#define COMMON_DIG_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int DIG_NO;

class Dig : public Generic
{
  public:
    Dig() {
        (*this)->setType("dig", DIG_NO);
    }
};

} } }

#endif // COMMON_DIG_H
