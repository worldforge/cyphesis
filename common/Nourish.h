// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

#include <Atlas/Objects/Operation/Action.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to nourish the entity which ate or burned another
/// entity
///
/// Typically sent in response to Burn or Eat operations, and used to
/// indicate how much the target entity benefits. In the Burn case, the
/// argument provides information about how much of the burnt entity has
/// been destroyted and how much the fire is increased. In the Eat case
/// it indicates how much nourishment the eaten entity provides to the
/// entity doing the eating.
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
