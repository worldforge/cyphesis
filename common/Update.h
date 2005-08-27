// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_UPDATE_H
#define COMMON_UPDATE_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class UpdateProxy {
  public:
    static const std::string name() { return "update"; }
};

typedef CustomOpData<SetData, UpdateProxy> UpdateData;

typedef SmartPtr<UpdateData> Update;

} } }

#endif // COMMON_UPDATE_H
