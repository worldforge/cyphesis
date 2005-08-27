// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_TICK_H
#define COMMON_TICK_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class TickProxy {
  public:
    static const std::string name() { return "tick"; }
};

typedef CustomOpData<SetData, TickProxy> TickData;

typedef SmartPtr<TickData> Tick;

} } }

#endif // COMMON_TICK_H
