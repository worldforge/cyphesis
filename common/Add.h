// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_ADD_H
#define COMMON_ADD_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class AddProxy {
  public:
    static const std::string name() { return "add"; }
};

typedef CustomOpData<SetData, AddProxy> AddData;

typedef SmartPtr<AddData> Add;

} } }

#endif // COMMON_ADD_H
