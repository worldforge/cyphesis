// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_CHOP_H
#define COMMON_CHOP_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

class Chop : public Action {
  public:
    Chop();
    virtual ~Chop();
    static Chop Instantiate();
};

} } }

#endif // COMMON_CHOP_H
