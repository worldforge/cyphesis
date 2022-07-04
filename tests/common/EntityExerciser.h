// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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


#ifndef TESTS_ENTITY_EXERCISER_H
#define TESTS_ENTITY_EXERCISER_H

#include "rules/LocatedEntity.h"

#define STUB_TypeNode_injectProperty

#include <Atlas/Objects/RootOperation.h>

#include <set>

#include <cassert>
#include <common/PropertyManager.h>

struct LocatedEntityTest : public LocatedEntity
{
    static PropertyManager* propertyManager;

    LocatedEntityTest(RouterId id) :
            LocatedEntity(id)
    {}

    void externalOperation(const Operation&, Link&) override;

    void operation(const Operation&, OpVector&) override;

    std::unique_ptr<PropertyBase> createProperty(const std::string& propertyName) const
    {
        if (propertyManager) {
            return propertyManager->addProperty(propertyName);
        }
        return {};
    }

};


class EntityExerciser
{
    protected:
        std::unique_ptr<TypeNode> test_type;
        Ref<LocatedEntity> m_parentEntity;
        Ref<LocatedEntity> m_ent;
        std::set<int> attr_types;
    public:
        explicit EntityExerciser(Ref<LocatedEntity> e);

        virtual ~EntityExerciser();

        bool checkAttributes(const std::set<std::string>& attr_names);

        virtual bool checkProperties(const std::set<std::string>& prop_names);

        bool fullAttributeTest(const Atlas::Message::MapType& attr_data);

        virtual void dispatchOp(const Atlas::Objects::Operation::RootOperation& op);

        void addAllOperations(std::set<std::string>& ops);

        virtual void runOperations();

        void runConversions();

        void flushOperations(OpVector& ops);
};

#endif // TESTS_ENTITY_EXERCISER_H
