// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#ifndef COMMON_TICK_H
#define COMMON_TICK_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int TICK_NO;

/// \brief An operation used to trigger one of a number of types of update.
/// \ingroup CustomOperations
class Tick : public Generic
{
  public:
    Tick() {
        (*this)->setType("tick", TICK_NO);
    }
};

} } }

#endif // COMMON_TICK_H
