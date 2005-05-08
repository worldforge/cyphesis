// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2005 Alistair Riddoch

#ifndef COMMON_MOW_H
#define COMMON_MOW_H

#include <Affect.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class sent by a tool to cut vegetation from target entity.
class Mow : public Affect {
  protected:
    Mow(const char *, const char *);
  public:
    Mow();
    virtual ~Mow();
    static Mow Class();
};

} } }

#endif // COMMON_MOW_H
