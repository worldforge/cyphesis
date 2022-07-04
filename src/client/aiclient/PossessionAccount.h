/*
 Copyright (C) 2015 Erik Ogenvik

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
#ifndef AICLIENT_POSSESSIONACCOUNT_H_
#define AICLIENT_POSSESSIONACCOUNT_H_

#include "common/Router.h"
#include "rules/ai/BaseMind.h"

#include <memory>
#include <unordered_set>
#include <functional>
#include <unordered_map>
#include <sigc++/connection.h>

class MindKit;

class TypeNode;

class PossessionClient;

class PossessionAccount : public Router
{
    public:
        PossessionAccount(RouterId id, const MindKit& mindFactory, PossessionClient& client);

        ~PossessionAccount() override;

        /**
         * Notify the server that we are able to possess minds.
         */
        void enablePossession(OpVector& res);

        void operation(const Operation& op, OpVector& res) override;

        void externalOperation(const Operation& op, Link&) override;

        const std::unordered_map<std::string, Ref<BaseMind>>& getMinds() const
        {
            return m_minds;
        };

        Ref<BaseMind> findMindForId(const std::string& id);

        static long account_count;
        static long mind_count;

    protected:
        PossessionClient& m_client;

        /**
         * Map of minds, with the key being the id of the actual mind.
         * This is separated from the m_entitiesWithMinds map, which contains mapping between the actual entity and the mind.
         * The idea is that we might want to do some differentiation between messages sent to the mind or the entity.
         */
        std::unordered_map<std::string, Ref<BaseMind>> m_minds;
        /**
         * Map of minds, with the key being the id of the entity to which the mind belongs to.
         * This is used to send ops that are directed to the entity itself to the mind that controls it.
         * Note that the current setup doesn't allow for multiple minds to control one entity.
         */
        std::unordered_map<std::string, Ref<BaseMind>> m_entitiesWithMinds;

        const MindKit& m_mindFactory;

        sigc::connection m_python_connection;


        void PossessOperation(const Operation& op, OpVector& res);

        void takePossession(OpVector& res, const std::string& possessEntityId, const std::string& possessKey);

        void createMindInstance(OpVector& res, RouterId mindId, const std::string& entityId);

};

#endif /* AICLIENT_POSSESSIONACCOUNT_H_ */
