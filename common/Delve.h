// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#ifndef COMMON_DELVE_H
#define COMMON_DELVE_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to direct the target tool to delve into the entity
/// given in the argument
class Delve : public Action {
  protected:
    Delve(const char *, const char *);
  public:
    Delve();
    virtual ~Delve();
    static Delve Class();
};

} } }

#endif // COMMON_DELVE_H
