// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_LOAD_H
#define COMMON_LOAD_H

#include <Atlas/Objects/Operation/Set.h>

namespace Atlas { namespace Objects { namespace Operation {

class Load : public Set {
  public:
    Load();
    virtual ~Load();
    static Load Instantiate();
};

} } }

#endif // COMMON_LOAD_H
