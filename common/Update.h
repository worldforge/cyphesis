// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_UPDATE_H
#define COMMON_UPDATE_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int UPDATE_NO;

class Update : public Generic
{
  public:
    Update() {
        (*this)->setType("update", UPDATE_NO);
    }
};

} } }

#endif // COMMON_UPDATE_H
