// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int SETUP_NO;

class Setup : public Generic
{
  public:
    Setup() {
        (*this)->setType("setup", SETUP_NO);
    }
};

} } }

#endif // COMMON_SETUP_H
