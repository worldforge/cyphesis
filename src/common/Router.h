// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef COMMON_ROUTER_H
#define COMMON_ROUTER_H

#include "OperationRouter.h"
#include "RouterId.h"

#include <string>
#include <map>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

class Link;

/// \brief This is the base class for any entity which has an Atlas
/// compatible identifier.
///
class Router {
  protected:
    explicit Router(RouterId id);
  public:
    Router(const Router &) = delete;
    virtual ~Router();

    const RouterId m_id;
    /// \brief Read only accessor for string identity
    const std::string & getId() const {
        return m_id.m_id;
    }

    /// \brief Read only accessor for Integer identity
    long getIntId() const {
        return m_id.m_intId;
    }

    void buildError(const Operation &,
                    const std::string & errstring,
                    const Operation &,
                    const std::string & to) const;
    void error(const Operation &, const std::string & errstring, OpVector &,
               const std::string & to = "") const;
    void clientError(const Operation &, const std::string & errstring,
                     OpVector &, const std::string & to = "") const;

    virtual void externalOperation(const Operation & op, Link &) = 0;
    virtual void operation(const Operation &, OpVector &) = 0;

    virtual void addToMessage(Atlas::Message::MapType &) const;
    virtual void addToEntity(const Atlas::Objects::Entity::RootEntity &) const;

    friend class Routertest;
};

#endif // COMMON_ROUTER_H
