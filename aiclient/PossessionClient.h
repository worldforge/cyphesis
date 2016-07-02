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
#include "LocatedEntityRegistry.h"
#include "common/OperationsDispatcher.h"
#include <map>
#include <unordered_map>

class MindKit;
class PossessionAccount;

/**
 * Manages possession requests from the server and spawns new AI clients.
 */
class PossessionClient: public BaseClient, public LocatedEntityRegistry
{
    public:
        PossessionClient(MindKit& mindFactory);
        virtual ~PossessionClient();

        bool idle();
        double secondsUntilNextOp() const;
        bool isQueueDirty() const;
        void markQueueAsClean();

        void createAccount(const std::string& accountId);

        virtual void addLocatedEntity(LocatedEntity* mind);
        virtual void removeLocatedEntity(LocatedEntity* mind);

    protected:

        virtual void operation(const Operation & op, OpVector & res);
        void operationFromEntity(const Operation & op, LocatedEntity& locatedEntity);
        double getTime() const;


        MindKit& m_mindFactory;

        PossessionAccount* m_account;

        OperationsDispatcher m_operationsDispatcher;

        std::unordered_map<long, LocatedEntity*> m_minds;

};

#endif /* POSSESSIONCLIENT_H_ */
