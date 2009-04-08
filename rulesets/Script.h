// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001-2004 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_SCRIPT_H
#define RULESETS_SCRIPT_H

#include <string>
#include <vector>

#include <Atlas/Objects/ObjectsFwd.h>

typedef std::vector<Atlas::Objects::Operation::RootOperation> OpVector;

class LocatedEntity;

/// \brief Base class for script objects attached to entities.
///
/// This base class allows scripts to override operation handlers, and
/// handlers for hooks.
class Script {
  public:
    Script();
    virtual ~Script();
    virtual bool operation(const std::string & opname,
                           const Atlas::Objects::Operation::RootOperation & op,
                           OpVector & res);
    virtual void hook(const std::string & function, LocatedEntity * entity);
};

extern Script noScript;

#endif // RULESETS_SCRIPT_H
