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
        PossessionAccount(const std::string& id, long intId, const MindKit& mindFactory, PossessionClient& client);

        ~PossessionAccount() override;

        /**
         * Notify the server that we are able to possess minds.
         */
        void enablePossession(OpVector& res);

        void operation(const Operation& op, OpVector& res) override;

        void externalOperation(const Operation& op, Link&) override;

        const std::unordered_map<std::string, Ref<BaseMind>>& getMinds() const {
            return m_minds;
        };

        Ref<BaseMind> findMindForId(const std::string& id);

    protected:
        PossessionClient& m_client;

        std::unordered_map<std::string, Ref<BaseMind>> m_minds;
        std::unordered_map<std::string, Ref<BaseMind>> m_entitiesWithMinds;

        const MindKit& m_mindFactory;

        sigc::connection m_python_connection;


        void PossessOperation(const Operation& op, OpVector& res);

        void takePossession(OpVector& res, const std::string& possessEntityId, const std::string& possessKey);

        void createMindInstance(OpVector& res, const std::string& mindId, const std::string& entityId);

};

#endif /* AICLIENT_POSSESSIONACCOUNT_H_ */
