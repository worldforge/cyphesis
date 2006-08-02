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

#ifndef COMMON_SET_H
#define COMMON_SET_H

#include <Atlas/Objects/Operation/Set.h>

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class to tell a character to wield a tool or weapon
///
/// Sent by a client or mind, this is not dispatched in the world.
class Wield : public Set {
  protected:
    Wield(const char *, const char *);
  public:
    Wield();
    virtual ~Wield();
    static Wield Class();
};

} } }

#endif // COMMON_SET_H
