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


#ifndef SERVER_RULE_HANDLER_H
#define SERVER_RULE_HANDLER_H

#include "common/TypeNode.h"

#include <Atlas/Objects/Root.h>

/// \brief Handle processing and updating of task ruless
class RuleHandler {
  public:
    virtual ~RuleHandler() = default;
    virtual int check(const Atlas::Objects::Root & desc) = 0;
    virtual int install(const std::string &,
                        const std::string &,
                        const Atlas::Objects::Root & desc,
                        std::string &,
                        std::string &,
                        std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) = 0;
    virtual int update(const std::string &,
                       const Atlas::Objects::Root & desc,
                       std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes) = 0;

};

#endif // SERVER_RULE_HANDLER_H
