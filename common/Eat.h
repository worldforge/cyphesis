// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

class Eat : public Action {
  protected:
    Eat(const char *, const char *);
  public:
    Eat();
    virtual ~Eat();
    static Eat Class();
};

} } }

#endif // COMMON_EAT_H
