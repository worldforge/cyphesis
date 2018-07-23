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

#include "common/BaseWorld.h"

#include "rulesets/LocatedEntity.h"
#include "rulesets/Task.h"

/// Test implementation of the BaseWorld interface
class TestWorld : public BaseWorld {
  public:
    LocatedEntity& m_gw;
    explicit TestWorld(LocatedEntity & gw) : BaseWorld(), m_gw(gw) {
        m_eobjects[gw.getIntId()] = &gw;
    }

    virtual ~TestWorld(){}

    virtual bool idle() { return false; }
    virtual Ref<LocatedEntity> addEntity(const Ref<LocatedEntity>& ent) {
        m_eobjects[ent->getIntId()] = ent.get();
        return 0;
    }
    virtual Ref<LocatedEntity> addNewEntity(const std::string &,
                                         const Atlas::Objects::Entity::RootEntity &);
    void delEntity(LocatedEntity * obj) {}
    int createSpawnPoint(const Atlas::Message::MapType & data,
                         LocatedEntity *) { return 0; }
    int removeSpawnPoint(LocatedEntity *) {return 0; }
    int getSpawnList(Atlas::Message::ListType & data) { return 0; }
    LocatedEntity * spawnNewEntity(const std::string & name,
                                   const std::string & type,
                                   const Atlas::Objects::Entity::RootEntity & desc) {
        return addNewEntity(type, desc);
    }
    virtual int moveToSpawn(const std::string & name,
                            Location& location){return 0;}
    virtual Ref<Task> newTask(const std::string &, LocatedEntity &) { return 0; }
    virtual Ref<Task> activateTask(const std::string &, const std::string &,
                                LocatedEntity *, LocatedEntity &) { return 0; }
    virtual ArithmeticScript * newArithmetic(const std::string &,
                                             LocatedEntity *) {
        return 0;
    }
    virtual void message(const Operation & op, LocatedEntity & ent);
    virtual void messageToClients(const Atlas::Objects::Operation::RootOperation &){};
    virtual LocatedEntity * findByName(const std::string & name) { return 0; }
    virtual LocatedEntity * findByType(const std::string & type) { return 0; }
    virtual void addPerceptive(LocatedEntity *) { }

    virtual LocatedEntity& getDefaultLocation() const {return m_gw;};

};

#endif // TESTS_TEST_WORLD_H
