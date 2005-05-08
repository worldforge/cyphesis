// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#ifndef COMMON_DIG_H
#define COMMON_DIG_H

#include <Affect.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class sent by a tool to dig into the target entity.
class Dig : public Affect {
  protected:
    Dig(const char *, const char *);
  public:
    Dig();
    virtual ~Dig();
    static Dig Class();
};

} } }

#endif // COMMON_DIG_H
