// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CUT_H
#define COMMON_CUT_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int CUT_NO;

class Cut : public Generic
{
  public:
    Cut() {
        (*this)->setType("cut", CUT_NO);
    }
};

} } }

#endif // COMMON_CUT_H
