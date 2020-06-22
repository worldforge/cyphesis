// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#ifndef TESTS_TEST_WORLD_H
#define TESTS_TEST_WORLD_H

#include "rules/simulation/BaseWorld.h"

#include "rules/LocatedEntity.h"
#include "rules/simulation/Task.h"
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>
#include <functional>
#include <utility>

struct TestWorldExtension {
    std::function<void(const Operation & op, LocatedEntity & ent)> messageFn;
    std::function<Ref<LocatedEntity>(const std::string &, const Atlas::Objects::Entity::RootEntity &)> addNewEntityFn;
};

struct TestWorld : public BaseWorld {

    static TestWorldExtension extension;
    TestWorldExtension m_extension;

    Ref<LocatedEntity> m_gw;
    explicit TestWorld() : BaseWorld(), m_gw(nullptr) {
    }
    explicit TestWorld(Ref<LocatedEntity> gw) : BaseWorld(), m_gw(std::move(gw)) {
        m_eobjects[m_gw->getIntId()] = m_gw;
    }

    ~TestWorld() override= default;

    void addEntity(const Ref<LocatedEntity>& ent, const Ref<LocatedEntity>& parent) override {
        m_eobjects[ent->getIntId()] = ent;
        if (parent) {
            parent->addChild(*ent);
        }
    }
    Ref<LocatedEntity> addNewEntity(const std::string & id,
                                         const Atlas::Objects::Entity::RootEntity & op) override
    {
        if (m_extension.addNewEntityFn) {
            return m_extension.addNewEntityFn(id, op);
        } else if (extension.addNewEntityFn) {
            return extension.addNewEntityFn(id, op);
        }
        return nullptr;
    }
    void delEntity(LocatedEntity * obj) override {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) override { return 0; }
    int removeSpawnPoint(LocatedEntity *) override {return 0; }
    int getSpawnList(Atlas::Message::ListType & data) override { return 0; }

     const std::set<std::string>& getSpawnEntities() const override {
        static std::set<std::string> spawns;
        return spawns;
    }

     void registerSpawner(const std::string& id) override {}

     void unregisterSpawner(const std::string& id) override {}

    Ref<LocatedEntity> spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) override
    {
        return addNewEntity(type, desc);
    }
    int moveToSpawn(const std::string & name,
                            Location& location) override{return 0;}

    void message(Operation op, LocatedEntity & ent) override {
        if (m_extension.messageFn) {
            m_extension.messageFn(op, ent);
        } else if (extension.messageFn) {
            extension.messageFn(op, ent);
        }
    }
    Ref<LocatedEntity> findByName(const std::string & name) override { return nullptr; }
    Ref<LocatedEntity> findByType(const std::string & type) override { return nullptr; }

    LocatedEntity& getDefaultLocation() const override {return *m_gw;};

};


#endif // TESTS_TEST_WORLD_H
