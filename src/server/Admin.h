// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef SERVER_ADMIN_H
#define SERVER_ADMIN_H

#include "Account.h"

#include <sigc++/connection.h>
#include <Atlas/Objects/SmartPtr.h>

/// \brief This is a class for handling users with administrative priveleges
class Admin : public Account
{
    protected:

        void opDispatched(const Operation& op);

        /// \brief Sets an attribute on the admin instance itself.
        void setAttribute(const Atlas::Objects::Root& arg);

        /// \brief Connection used to monitor the in-game operations
        sigc::connection m_monitorConnection;

        std::unique_ptr<ExternalMind> createMind(const Ref<LocatedEntity>& entity) const override;

        void processExternalOperation(const Operation& op, OpVector& res) override;

    public:
        Admin(Connection* conn, const std::string& username,
              const std::string& passwd,
              RouterId id);

        ~Admin() override;

        const char* getType() const override;

        /**
         * Allow admin clients to logout other accounts.
         */
        void LogoutOperation(const Operation&, OpVector&) override;

        void GetOperation(const Operation&, OpVector&) override;

        void CreateOperation(const Operation&, OpVector&) override;

        void SetOperation(const Operation&, OpVector&) override;

        void OtherOperation(const Operation&, OpVector&) override;

        void customMonitorOperation(const Operation&, OpVector&);

        friend class Admintest;
};

#endif // SERVER_ADMIN_H
