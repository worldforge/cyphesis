// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

#include <map>

namespace Atlas {
  namespace Message {
    class Element;
    typedef std::map<std::string, Element> MapType;
  }
  namespace Objects {
    class Root;
    namespace Operation {
      class RootOperation;
    }
  }
}

namespace utility {

  Atlas::Objects::Root * Object_asRoot(const Atlas::Message::Element &);
  bool Object_asOperation(const Atlas::Message::MapType &,
                          Atlas::Objects::Operation::RootOperation &);

}

#endif // COMMON_UTILITY_H
