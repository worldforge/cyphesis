// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_CHOP_H
#define COMMON_CHOP_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to chop the target entity
class Chop : public Action {
  protected:
    Chop(const char *, const char *);
  public:
    Chop();
    virtual ~Chop();
    static Chop Class();
};

} } }

#endif // COMMON_CHOP_H
