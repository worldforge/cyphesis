// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_MONITOR_H
#define COMMON_MONITOR_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int MONITOR_NO;

class Monitor : public Generic
{
  public:
    Monitor() {
        (*this)->setType("monitor", MONITOR_NO);
    }
};

} } }

#endif // COMMON_MONITOR_H
