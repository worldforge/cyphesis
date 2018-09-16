/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_PENDINGMIND_H
#define CYPHESIS_PENDINGMIND_H


#include "common/Router.h"
#include "rulesets/TypeResolver.h"

#include <Atlas/Objects/Operation.h>
#include <memory>
class MindRegistry;
class MindKit;

/**
 * This class is used to represent a mind that's in the process of being created, where we have the entity data
 * but are missing the type data. Once all type data has been received an actual BaseMind instance will be created
 * and this instance destroyed.
 */
class PendingMind
{
    public:
        PendingMind(std::string entityId, std::string mindId, MindRegistry& locatedEntityRegistry, const MindKit& mindFactory);

        void operation(const Operation& op, OpVector& res);

        void init(OpVector& res);

    private:

        std::string m_entityId;
        std::string m_mindId;
        MindRegistry& m_mindRegistry;
        const MindKit& m_mindFactory;

        std::string m_typeName;

        Atlas::Objects::Operation::Sight m_entitySight;

        std::vector<Operation> m_pendingOperations;

        long m_serialNoCounter;

        std::unique_ptr<TypeResolver> m_typeResolver;

        void createEntity(OpVector& res, const TypeNode* type);
};


#endif //CYPHESIS_PENDINGMIND_H
