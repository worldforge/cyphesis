// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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

#ifndef COMMON_OOG_THING_H
#define COMMON_OOG_THING_H

#include "BaseEntity.h"

/// \brief This is the base class from which all OOG entity classes inherit.
///
/// This class only overrides OtherOperation() ensuring that operations on
/// unknown types flag an error.
class OOGThing : public BaseEntity {
  protected:
    explicit OOGThing(const std::string & id, long intId);
  public:
    virtual ~OOGThing();
    virtual void OtherOperation(const Operation & op, OpVector &);
};

#endif // COMMON_OOG_THING_H
