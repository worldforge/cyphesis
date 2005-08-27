// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class SetupProxy {
  public:
    static const std::string name() { return "setup"; }
};

typedef CustomOpData<SetData, SetupProxy> SetupData;

typedef SmartPtr<SetupData> Setup;

} } }

#endif // COMMON_SETUP_H
