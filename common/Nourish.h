// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

class Nourish : public Action {
  protected:
    Nourish(const char *, const char *);
  public:
    Nourish();
    virtual ~Nourish();
    static Nourish Class();
};

} } }

#endif // COMMON_NOURISH_H
