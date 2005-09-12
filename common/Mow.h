// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_MOW_H
#define COMMON_MOW_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int MOW_NO;

class Mow : public Generic
{
  public:
    Mow() {
        (*this)->setType("mow", MOW_NO);
    }
};

} } }

#endif // COMMON_MOW_H
