// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#include <Atlas/Objects/Operation/RootOperation.h>

namespace utility {

  Atlas::Objects::Root * Object_asRoot(const Atlas::Message::Element &);
  bool Object_asOperation(const Atlas::Message::Element::MapType &,
                          Atlas::Objects::Operation::RootOperation &);

}

#endif // COMMON_UTILITY_H
