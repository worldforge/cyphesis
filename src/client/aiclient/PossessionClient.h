/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef POSSESSIONCLIENT_H_
#define POSSESSIONCLIENT_H_

#include "client/BaseClient.h"
#include "rules/ai/BaseMind.h"
#include "common/OperationsDispatcher.h"
#include "common/OperationsDispatcher_impl.h"
#include <map>
#include <unordered_map>

class MindKit;

class PossessionAccount;

class Inheritance;

/**
 * Manages possession requests from the server and spawns new AI clients.
 */
class PossessionClient : public BaseClient
{
    public:
        explicit PossessionClient(CommSocket& commSocket,
                                  MindKit& mindFactory,
                                  std::unique_ptr<Inheritance> inheritance,
                                  std::function<void()> reconnectFn);

        ~PossessionClient() override;

        const std::unordered_map<std::string, Ref<BaseMind>>& getMinds() const;

        void processResponses(const OpVector& incomingRes, OpVector& outgoingRes);

        static long operations_in;
        static long operations_out;

    protected:

        void operation(const Operation& op, OpVector& res) override;

        void processOperation(const Operation& op, OpVector& res);

        void operationFromEntity(const Operation& op, Ref<BaseMind> locatedEntity);

        std::chrono::steady_clock::duration getTime() const;

        std::chrono::steady_clock::time_point m_startTime;

        void scheduleDispatch();

        void notifyAccountCreated(RouterId accountId) override;

        void resolveDispatchTimeForOp(Atlas::Objects::Operation::RootOperationData& op);

        MindKit& m_mindFactory;

        std::function<void()> m_reconnectFn;

        std::unique_ptr<PossessionAccount> m_account;

        OperationsDispatcher<BaseMind> m_operationsDispatcher;

        std::unique_ptr<Inheritance> m_inheritance;

        boost::asio::steady_timer m_dispatcherTimer;

        /**
         * Keep track of the difference between the server time and our local time.
         * This is needed when scheduling operations locally. When they are later dispatched
         * their "seconds" needs to match the time it would be on the server.
         */
        double m_serverLocalTimeDiff;


};

#endif /* POSSESSIONCLIENT_H_ */
