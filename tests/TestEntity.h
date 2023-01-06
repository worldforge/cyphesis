/*
 Copyright (C) 2020 Erik Ogenvik

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

#ifndef TESTS_TEST_ENTITY_H
#define TESTS_TEST_ENTITY_H

#include "rules/LocatedEntity.h"

struct TestEntity : LocatedEntity
{
    static long idGenerator;

    static std::function<void(TestEntity&, const Operation&, Link&)> externalOperationFn;
    static std::function<void(TestEntity&, const Operation&, OpVector&)> operationFn;
    static std::function<void(TestEntity&)> destroyFn;

    TestEntity() : TestEntity(idGenerator++)
    {
    }

    TestEntity(RouterId id) : LocatedEntity(id)
    {
    }

    std::unique_ptr<PropertyBase> createProperty(const std::string& propertyName) const override
    {
        return {};
    }

    void externalOperation(const Operation& op, Link& link) override
    {
        if (externalOperationFn) {
            externalOperationFn(*this, op, link);
        }
    }

    void operation(const Operation& op, OpVector& res) override
    {
        if (operationFn) {
            operationFn(*this, op, res);
        }
    }

    void destroy() override
    {
        if (destroyFn) {
            destroyFn(*this);
        }
    }
};

long TestEntity::idGenerator = 0;

std::function<void(TestEntity&, const Operation&, Link&)> TestEntity::externalOperationFn;
std::function<void(TestEntity&, const Operation&, OpVector&)> TestEntity::operationFn;
std::function<void(TestEntity&)> TestEntity::destroyFn;

#endif //TESTS_TEST_ENTITY_H

