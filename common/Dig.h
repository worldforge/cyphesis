// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#ifndef COMMON_DIG_H
#define COMMON_DIG_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to direct the target tool to dig into the entity
/// given in the argument
class Dig : public Action {
  protected:
    Dig(const char *, const char *);
  public:
    Dig();
    virtual ~Dig();
    static Dig Class();
};

} } }

#endif // COMMON_DIG_H
