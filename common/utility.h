// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#include "operations.h"

namespace utility {

  Atlas::Objects::Root * Object_asRoot(const Element &);
  bool Object_asOperation(const MapType &, RootOperation &);

}

#endif // COMMON_UTILITY_H
