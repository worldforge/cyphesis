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

#include "BaseClient.h"
#include "MindRegistry.h"
#include "rulesets/BaseMind.h"
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
class PossessionClient: public BaseClient, public MindRegistry
{
    public:
        explicit PossessionClient(MindKit& mindFactory);

        ~PossessionClient() override = default;

        bool idle();
        double secondsUntilNextOp() const;
        bool isQueueDirty() const;
        void markQueueAsClean();

        void createAccount(const std::string& accountId);

        void addLocatedEntity(BaseMind* mind) override;

        void removeLocatedEntity(BaseMind* mind) override;

        const std::unordered_map<long, BaseMind*>& getMinds() const {
            return m_minds;
        };

    protected:

        void operation(const Operation & op, OpVector & res) override;
        void operationFromEntity(const Operation & op, BaseMind& locatedEntity);
        double getTime() const;


        MindKit& m_mindFactory;

        PossessionAccount* m_account;

        OperationsDispatcher<BaseMind> m_operationsDispatcher;

        std::unordered_map<long, BaseMind*> m_minds;

        std::unique_ptr<Inheritance> m_inheritance;

};

#endif /* POSSESSIONCLIENT_H_ */
