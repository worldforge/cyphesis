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

// $Id: Eat.h,v 1.16 2006-12-12 15:54:22 alriddoch Exp $

#ifndef COMMON_EAT_H
#define COMMON_EAT_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int EAT_NO;

/// \brief An action and affect operation used to initiate and indicate
/// something is being eaten.
/// \ingroup CustomOperations
class Eat : public Generic
{
  public:
    Eat() {
        (*this)->setType("eat", EAT_NO);
    }
};

} } }

#endif // COMMON_EAT_H
