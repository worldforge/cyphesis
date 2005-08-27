// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_MONITOR_H
#define COMMON_MONITOR_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class MonitorProxy {
  public:
    static const std::string name() { return "monitor"; }
};

typedef CustomOpData<SetData, MonitorProxy> MonitorData;

typedef SmartPtr<MonitorData> Monitor;

} } }

#endif // COMMON_MONITOR_H
