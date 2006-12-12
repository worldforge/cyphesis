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

// $Id: Burn.h,v 1.10 2006-12-12 15:54:21 alriddoch Exp $

#ifndef COMMON_BURN_H
#define COMMON_BURN_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int BURN_NO;

/// \brief An affect operation used to indicate something is affected by fire.
/// \ingroup CustomOperations
class Burn : public Generic
{
  public:
    Burn() {
        (*this)->setType("burn", BURN_NO);
    }
};

} } }

#endif // COMMON_BURN_H
