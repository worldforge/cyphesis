// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_AFFECT_H
#define COMMON_AFFECT_H

#include "common/CustomOp.h"

#include <Atlas/Objects/ObjectsFwd.h>

namespace Atlas { namespace Objects { namespace Operation {

typedef CustomOpData<SetData> AffectData;

typedef SmartPtr<AffectData> Affect;

} } }

#endif // COMMON_AFFECT_H
