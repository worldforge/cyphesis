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


#ifndef CLIENT_CLIENT_PROPERTY_MANAGER_H
#define CLIENT_CLIENT_PROPERTY_MANAGER_H

#include "common/PropertyManager.h"

/// \brief Property manager for the client where no properties are required.
class ClientPropertyManager : public PropertyManager {
  public:
    ClientPropertyManager();
    virtual ~ClientPropertyManager();

    virtual PropertyBase * addProperty(const std::string & name, int);
};

#endif // CLIENT_CLIENT_PROPERTY_MANAGER_H
