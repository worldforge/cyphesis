// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_AFFECT_H
#define COMMON_AFFECT_H

#include <Atlas/Objects/Operation/Set.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation base class to alter the target entity in some high level
/// way.
class Affect : public Set {
  protected:
    Affect(const char *, const char *);
  public:
    Affect();
    virtual ~Affect();
    static Affect Class();
};

} } }

#endif // COMMON_AFFECT_H
