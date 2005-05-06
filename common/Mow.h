// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#ifndef COMMON_MOW_H
#define COMMON_MOW_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to direct the target tool to mow vegetation from
/// the entity given in the argument
class Mow : public Action {
  protected:
    Mow(const char *, const char *);
  public:
    Mow();
    virtual ~Mow();
    static Mow Class();
};

} } }

#endif // COMMON_MOW_H
