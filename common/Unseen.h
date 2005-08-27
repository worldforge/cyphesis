// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_UNSEEN_H
#define COMMON_UNSEEN_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

class UnseenProxy {
  public:
    static const std::string name() { return "unseen"; }
};

typedef CustomOpData<SetData, UnseenProxy> UnseenData;

typedef SmartPtr<UnseenData> Unseen;

} } }

#endif // COMMON_UNSEEN_H
