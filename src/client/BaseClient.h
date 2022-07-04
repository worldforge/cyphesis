// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
// Copyright (C) 2013 Erik Ogenvik
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


#ifndef CLIENT_BASE_CLIENT_H
#define CLIENT_BASE_CLIENT_H

#include "common/Link.h"
#include <memory>
#include <boost/asio/steady_timer.hpp>

class CreatorClient;
class ClientTask;

/// \brief Base class for classes that implement clients used to connect to a
/// cyphesis server
class BaseClient : public Link
{
    protected:

        std::shared_ptr<ClientTask> m_task;

        std::string m_username;
        std::string m_password;

        long m_serialNo;

        struct CallbackEntry
        {
            std::function<void(const Operation&, OpVector&)> callback;
            std::unique_ptr<boost::asio::steady_timer> timeout;
            std::function<void()> timeoutCallback;
        };

        std::map<long, CallbackEntry> m_callbacks;


        static std::string getErrorMessage(const Operation& err);

        virtual void notifyAccountCreated(RouterId accountId) = 0;

    public:
        explicit BaseClient(CommSocket& commSocket);

        ~BaseClient() override = default;

        void createSystemAccount(const std::string& usernameSuffix = "");

        Atlas::Objects::Root createAccount(const std::string& name,
                                           const std::string& pword);

        CreatorClient* createCharacter(const std::string& name);

        void logout();

        void externalOperation(const Operation& op, Link&) override;


        int runTask(std::shared_ptr<ClientTask> task, const std::string& arg);
        int runTask(std::function<bool(const Operation&, OpVector&)> function);

        int endTask();

        void sendWithCallback(Operation op, std::function<void(const Operation&, OpVector&)> callback,
                              std::function<void()> timeoutCallback = std::function<void()>(),
                              std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));

        /**
         * Checks if there's an active task.
         * @return True if there's a task set.
         */
        bool hasTask() const;

        void notifyConnectionComplete() override;

};

#endif // CLIENT_BASE_CLIENT_H
