// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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


#ifndef COMMON_SCRIPT_KIT_H
#define COMMON_SCRIPT_KIT_H

#include <string>

/// \brief Factory interface for creating scripts to attach to in game
/// entity objects.
template <class T>
class ScriptKit {
  public:
    virtual ~ScriptKit() = default;

    /// \brief Accessor for package name
    virtual const std::string & package() const = 0;
    /// \brief Add a script to an entity
    virtual int addScript(T& entity) const = 0;
    /// \brief Reload the underlying class object from the script on disk
    virtual int refreshClass() = 0;
};


#endif // COMMON_SCRIPT_KIT_H
