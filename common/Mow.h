// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_MOW_H
#define COMMON_MOW_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class MowProxy {
  public:
    static const std::string name() { return "mow"; }
};

typedef CustomOpData<SetData, MowProxy> MowData;

typedef SmartPtr<MowData> Mow;

} } }

#endif // COMMON_MOW_H
