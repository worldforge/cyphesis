// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#include "BaseWorld.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "rules/Script.h"
#include "rules/LocatedEntity.h"

#include <cassert>
#include <utility>

static const bool debug_flag = false;

/// \brief BaseWorld constructor.
///
/// Protected as BaseWorld is a base class.
BaseWorld::BaseWorld(TimeProviderFnType timeProviderFn) :
        m_timeProviderFn(std::move(timeProviderFn)),
        m_initTime(std::chrono::steady_clock::now()),
        m_isSuspended(false)
{
}

BaseWorld::~BaseWorld() {
    shutdown();
}

void BaseWorld::shutdown()
{
    if (!m_eobjects.empty()) {
        debug_print("Flushing world with " << m_eobjects.size() << " entities");
        // Make sure that no entity references are retained.
        for (const auto& entry : m_eobjects) {
            entry.second->m_parent = nullptr;
            if (entry.second->m_contains) {
                entry.second->m_contains->clear();
            }
            entry.second->clearProperties();
            // Set the type to null so we won't clear properties again in the destructor.
            entry.second->setType(nullptr);
        }

        m_eobjects.clear();
    }
}


/// \brief Get an in-game Entity by its string ID.
///
/// @param id string ID of Entity to be retrieved.
/// @return pointer to Entity retrieved, or zero if it was not found.
Ref<LocatedEntity> BaseWorld::getEntity(const std::string& id) const
{
    return getEntity(integerId(id));
}

/// \brief Get an in-game Entity by its integer ID.
///
/// @param id integer ID of Entity to be retrieved.
/// @return pointer to Entity retrieved, or zero if it was not found.
Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}

void BaseWorld::registerAlias(std::string alias, LocatedEntity& entity)
{
    auto I = m_entityAliases.emplace(alias, &entity);
    if (!I.second) {
        if (I.first->second != &entity) {
            log(WARNING, String::compose("Tried to register entity %1 with alias %2, which already is connected to entity %3.", entity.describeEntity(), alias, I.first->second->describeEntity()));
        }
    }
}

void BaseWorld::deregisterAlias(const std::string& alias, LocatedEntity& entity)
{
    auto I = m_entityAliases.find(alias);
    if (I != m_entityAliases.end()) {
        if (I->second == &entity) {
            m_entityAliases.erase(I);
        } else {
            log(WARNING, String::compose("Tried to deregister entity %1 from alias %2, which is connected to entity %3.", entity.describeEntity(), alias, I->second->describeEntity()));
        }
    } else {
        log(WARNING, String::compose("Tried to deregister entity %1 from alias %2, which has no entry.", entity.describeEntity(), alias));
    }
}

LocatedEntity* BaseWorld::getAliasEntity(const std::string& alias) const
{
    auto I = m_entityAliases.find(alias);
    if (I != m_entityAliases.end()) {
        return I->second;
    }
    return nullptr;
}

void BaseWorld::setIsSuspended(bool suspended)
{
    bool wasSuspended = m_isSuspended;
    m_isSuspended = suspended;
    if (!suspended && wasSuspended) {
        log(INFO, "Resuming world.");
        resumeWorld();
    } else {
        log(INFO, "Suspending world.");
    }
}

std::chrono::steady_clock::duration BaseWorld::getTime() const
{
    return m_timeProviderFn();
//    return (std::chrono::steady_clock::now() - m_initTime);
}

float BaseWorld::getTimeAsSeconds() const
{
    return std::chrono::duration_cast<std::chrono::duration<float>>(getTime()).count();
}

