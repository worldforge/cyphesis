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

// $Id$

#ifndef SERVER_RULE_HANDLER_H
#define SERVER_RULE_HANDLER_H

#include <Atlas/Objects/Root.h>

/// \brief Handle processing and updating of task ruless
class RuleHandler {
  public:
    virtual ~RuleHandler();
    virtual int check(const Atlas::Objects::Root & desc) = 0;
    virtual int install(const std::string &,
                        const std::string &,
                        const Atlas::Objects::Root & desc,
                        std::string &,
                        std::string &) = 0;
    virtual int update(const std::string &,
                       const Atlas::Objects::Root & desc) = 0;

    static int getScriptDetails(const Atlas::Message::MapType &,
                                const std::string &,
                                const std::string &,
                                std::string &,
                                std::string &);
};

#endif // SERVER_RULE_HANDLER_H
