// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_DELVE_H
#define COMMON_DELVE_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int DELVE_NO;

class Delve : public Generic
{
  public:
    Delve() {
        (*this)->setType("delve", DELVE_NO);
    }
};

} } }

#endif // COMMON_DELVE_H
