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

#include <memory>
#include <unordered_set>

class MindKit;
class LocatedEntityRegistry;

class PossessionAccount: public Router
{
    public:
        PossessionAccount(const std::string& id, int intId, LocatedEntityRegistry& locatedEntityRegistry, const MindKit& mindFactory);
        virtual ~PossessionAccount();

        /**
         * Notify the server that we are able to possess minds.
         */
        void enablePossession(OpVector& res);

        virtual void operation(const Operation & op, OpVector & res);
        virtual void externalOperation(const Operation & op, Link &);

    protected:
        LocatedEntityRegistry& mLocatedEntityRegistry;
        const MindKit& m_mindFactory;

        int m_serialNoCounter;

        std::unordered_set<int> m_possessionRefNumbers;

        void PossessOperation(const Operation & op, OpVector & res);

        void takePossession(OpVector& res, const std::string& possessEntityId, const std::string& possessKey);
        void createMind(const Operation & op, OpVector & res);

};

#endif /* AICLIENT_POSSESSIONACCOUNT_H_ */
