// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_DELVE_H
#define COMMON_DELVE_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class DelveProxy {
  public:
    static const std::string name() { return "delve"; }
};

typedef CustomOpData<SetData, DelveProxy> DelveData;

typedef SmartPtr<DelveData> Delve;

} } }

#endif // COMMON_DELVE_H
