// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int EAT_NO;

class Eat : public Generic
{
  public:
    Eat() {
        (*this)->setType("eat", EAT_NO);
    }
};

} } }

#endif // COMMON_EAT_H
