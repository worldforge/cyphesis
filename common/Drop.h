// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
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

// $Id: Drop.h,v 1.4 2006-12-12 15:54:22 alriddoch Exp $

#ifndef COMMON_DROP_H
#define COMMON_DROP_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int DROP_NO;

/// \brief An action operation used to initiate putting an inventory item
/// down.
/// \ingroup CustomOperations
class Drop : public Generic
{
  public:
    Drop() {
        (*this)->setType("drop", DROP_NO);
    }
};

} } }

#endif // COMMON_DROP_H
