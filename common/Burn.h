// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_BURN_H
#define COMMON_BURN_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class BurnProxy {
  public:
    static const std::string name() { return "burn"; }
};

typedef CustomOpData<SetData, BurnProxy> BurnData;

typedef SmartPtr<BurnData> Burn;

} } }

#endif // COMMON_BURN_H
