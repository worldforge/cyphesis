// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_BURN_H
#define COMMON_BURN_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to burn the target entity
class Burn : public Action {
  protected:
    Burn(const char *, const char *);
  public:
    Burn();
    virtual ~Burn();
    static Burn Class();
};

} } }

#endif // COMMON_BURN_H
