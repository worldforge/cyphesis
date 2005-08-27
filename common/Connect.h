// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CONNECT_H
#define COMMON_CONNECT_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class ConnectProxy {
  public:
    static const std::string name() { return "connect"; }
};

typedef CustomOpData<SetData, ConnectProxy> ConnectData;

typedef SmartPtr<ConnectData> Connect;

} } }

#endif // COMMON_CONNECT_H
