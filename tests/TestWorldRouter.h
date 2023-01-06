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

#ifndef TESTS_TEST_WORLD_ROUTER_H
#define TESTS_TEST_WORLD_ROUTER_H

#include <rules/simulation/WorldRouter.h>

struct TestWorldRouter : public WorldRouter
{
    struct TestWorldTestWorldRouter
    {
        std::function<void(const Operation& op, LocatedEntity& ent)> messageFn;
        std::function<Ref<LocatedEntity>(const std::string&, const Atlas::Objects::Entity::RootEntity&)> addNewEntityFn;
    };

    TestWorldTestWorldRouter m_extension;

    explicit TestWorldRouter(Ref<LocatedEntity> gw, EntityCreator& entityCreator)
            : WorldRouter(std::move(gw), entityCreator, [] { return std::chrono::steady_clock::now().time_since_epoch(); })
    {
    }

    ~TestWorldRouter() override = default;

    Ref<LocatedEntity> addNewEntity(const std::string& id, const Atlas::Objects::Entity::RootEntity& op) override
    {
        if (m_extension.addNewEntityFn) {
            return m_extension.addNewEntityFn(id, op);
        } else {
            return WorldRouter::addNewEntity(id, op);
        }
    }

    void message(Operation op, LocatedEntity& ent) override
    {
        if (m_extension.messageFn) {
            m_extension.messageFn(op, ent);
        } else {
            WorldRouter::message(op, ent);
        }
    }
};

#endif //TESTS_TEST_WORLD_ROUTER_H
