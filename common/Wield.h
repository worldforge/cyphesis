// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_SET_H
#define COMMON_SET_H

#include <Atlas/Objects/Operation/Set.h>

namespace Atlas { namespace Objects { namespace Operation {

class Wield : public Set {
  protected:
    Wield(const char *, const char *);
  public:
    Wield();
    virtual ~Wield();
    static Wield Class();
};

} } }

#endif // COMMON_SET_H
