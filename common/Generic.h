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

#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef COMMON_GENERIC_H
#define COMMON_GENERIC_H

#include <Atlas/Objects/Operation/RootOperation.h>

// This operation cannot be instantiated directly, it can only be given
// a parent string in order to create an operation instance with
// parents that none of the hardcoded types support. Operations of
// this type should only be created and interpreted in scripts. If
// operations must be understood by C++ code, then add a new class.

namespace Atlas { namespace Objects { namespace Operation {

/// \brief Operation class for operations generated from scripts for which
/// there is no hard-coded class
class Generic : public RootOperation {
  public:
    explicit Generic(const std::string & p);
    virtual ~Generic();
    static Generic Class(const std::string & p);
};

} } }

#endif // COMMON_GENERIC_H
