// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class EatProxy {
  public:
    static const std::string name() { return "eat"; }
};

typedef CustomOpData<SetData, EatProxy> EatData;

typedef SmartPtr<EatData> Eat;

} } }

#endif // COMMON_EAT_H
