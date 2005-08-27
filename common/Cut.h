// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_CUT_H
#define COMMON_CUT_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class CutProxy {
  public:
    static const std::string name() { return "cut"; }
};

typedef CustomOpData<SetData, CutProxy> CutData;

typedef SmartPtr<CutData> Cut;

} } }

#endif // COMMON_CUT_H
