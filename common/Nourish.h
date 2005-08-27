// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class NourishProxy {
  public:
    static const std::string name() { return "nourish"; }
};

typedef CustomOpData<SetData, NourishProxy> NourishData;

typedef SmartPtr<NourishData> Nourish;

} } }

#endif // COMMON_NOURISH_H
