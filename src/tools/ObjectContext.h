// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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


#ifndef TOOLS_OBJECT_CONTEXT_H
#define TOOLS_OBJECT_CONTEXT_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <string>
#include <memory>

class Interactive;

class ObjectContext : public std::enable_shared_from_this<ObjectContext>
{
  protected:
    Interactive & m_client;
  public:
    explicit ObjectContext(Interactive & i) : m_client(i) { }
    virtual ~ObjectContext() = default;
    virtual bool accept(const Atlas::Objects::Operation::RootOperation&) const = 0;
    virtual int dispatch(const Atlas::Objects::Operation::RootOperation&) = 0;
    virtual std::string repr() const = 0;
    virtual bool checkContextCommand(const struct command *) = 0;
    virtual void setFromContext(const Atlas::Objects::Operation::RootOperation&) = 0;
};

#endif // TOOLS_OBJECT_CONTEXT_H
