// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_ADD_H
#define COMMON_ADD_H

#include <Atlas/Objects/Operation/Set.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to incrementally modify the attributes of an entity
///
/// This operation is important as it avoids the race conditions that are
/// so common when modifying an attribute using Set.
class Add : public Set {
  protected:
    Add(const char *, const char *);
  public:
    Add();
    virtual ~Add();
    static Add Class();
};

} } }

#endif // COMMON_ADD_H
