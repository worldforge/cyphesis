// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

#include <Atlas/Objects/Operation/RootOperation.h>

namespace Atlas { namespace Objects { namespace Operation {

class Setup : public RootOperation {
  protected:
    Setup(const char *, const char *);
  public:
    Setup();
    virtual ~Setup();
    static Setup Class();
};

} } }

#endif // COMMON_SETUP_H
