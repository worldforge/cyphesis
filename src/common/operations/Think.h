// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Erik Ogenvik
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


#ifndef COMMON_THINK_H
#define COMMON_THINK_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int THINK_NO;

/// \brief A "set" operation used to author entities minds.
///
/// This operation is used to send "thoughts" to an entity's mind.
/// \see Commune
/// \ingroup CustomOperations
class Think : public Generic
{
  public:
        Think() {
        (*this)->setType("think", THINK_NO);
    }
};

} } }

#endif // COMMON_THINK_H
