// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_UTILITY_H
#define COMMON_UTILITY_H

using Atlas::Message::Object;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::RootOperation;

namespace utility {

Root * Object_asRoot(const Atlas::Message::Object &);

}

#endif /* COMMON_UTILITY_H */
