// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_UNSEEN_H
#define COMMON_UNSEEN_H

#include <Atlas/Objects/Operation/Perception.h>

namespace Atlas { namespace Objects { namespace Operation {

class Unseen : public Perception {
  protected:
    Unseen(const char *, const char *);
  public:
    Unseen();
    virtual ~Unseen();
    static Unseen Class();
};

} } }

#endif // COMMON_UNSEEN_H
