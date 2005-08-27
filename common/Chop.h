// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CHOP_H
#define COMMON_CHOP_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class ChopProxy {
  public:
    static const std::string name() { return "chop"; }
};

typedef CustomOpData<SetData, ChopProxy> ChopData;

typedef SmartPtr<ChopData> Chop;

} } }

#endif // COMMON_CHOP_H
