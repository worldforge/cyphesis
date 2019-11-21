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


#ifndef TOOLS_ACCOUNT_CONTEXT_H
#define TOOLS_ACCOUNT_CONTEXT_H

#include "IdContext.h"

class AccountContext : public IdContext
{
    protected:
        const std::string m_username;
    public:
        AccountContext(Interactive&,
                       std::string id,
                       std::string u);

        bool accept(const Atlas::Objects::Operation::RootOperation&) const override;

        int dispatch(const Atlas::Objects::Operation::RootOperation&) override;

        std::string repr() const override;

        bool checkContextCommand(const struct command*) override;
};

#endif // TOOLS_ACCOUNT_CONTEXT_H
