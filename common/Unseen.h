// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_UNSEEN_H
#define COMMON_UNSEEN_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int UNSEEN_NO;

class Unseen : public Generic
{
  public:
    Unseen() {
        (*this)->setType("unseen", UNSEEN_NO);
    }
};

} } }

#endif // COMMON_UNSEEN_H
