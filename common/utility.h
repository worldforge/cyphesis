// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#include <Atlas/Objects/Operation.h>
#include <map>

namespace Atlas {
  namespace Message {
    class Element;
    typedef std::map<std::string, Element> MapType;
  }
}

namespace utility {

  bool Object_asOperation(const Atlas::Message::MapType &,
                          Atlas::Objects::Operation::RootOperation &);

}

#endif // COMMON_UTILITY_H
