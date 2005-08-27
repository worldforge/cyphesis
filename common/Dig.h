// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_DIG_H
#define COMMON_DIG_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class DigProxy {
  public:
    static const std::string name() { return "dig"; }
};

typedef CustomOpData<SetData, DigProxy> DigData;

typedef SmartPtr<DigData> Dig;

} } }

#endif // COMMON_DIG_H
