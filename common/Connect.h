// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CONNECT_H
#define COMMON_CONNECT_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int CONNECT_NO;

class Connect : public Generic
{
  public:
    Connect() {
        (*this)->setType("connect", CONNECT_NO);
    }
};

} } }

#endif // COMMON_CONNECT_H
