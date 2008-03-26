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

// $Id: Actuate.h,v 1.1 2008-03-26 01:34:16 alriddoch Exp $

#ifndef COMMON_ACTUATE_H
#define COMMON_ACTUATE_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int ACTUATE_NO;

/// \brief An operation used to actuate a feature of a device.
/// \ingroup CustomOperations
class Actuate : public Generic
{
  public:
    Actuate() {
        (*this)->setType("actuate", ACTUATE_NO);
    }
};

} } }

#endif // COMMON_ACTUATE_H
