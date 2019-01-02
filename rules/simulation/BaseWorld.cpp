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
#include "common/SystemTime.h"
#include "common/debug.h"
#include "rules/Script.h"
#include "rules/LocatedEntity.h"

#include <cassert>
static const bool debug_flag = false;

template<> BaseWorld* Singleton<BaseWorld>::ms_Singleton = nullptr;

/// \brief BaseWorld constructor.
///
/// Protected as BaseWorld is a base class.
BaseWorld::BaseWorld() :
    m_isSuspended(false),
    m_defaultLocation(nullptr),
    m_limboLocation(nullptr)
{
}

void BaseWorld::shutdown()
{
    debug(std::cout << "Flushing world with " << m_eobjects.size()
                    << " entities" << std::endl << std::flush;);
    //Make sure that no entity references are retained.
    for (const auto& entry : m_eobjects) {
        entry.second->m_location.m_parent = nullptr;
        if (entry.second->m_contains) {
            entry.second->m_contains->clear();
        }
        entry.second->clearProperties();
        //Set the type to null so we won't clear properties again in the destructor.
        entry.second->setType(nullptr);
    }

    m_eobjects.clear();
}


/// \brief Get an in-game Entity by its string ID.
///
/// @param id string ID of Entity to be retrieved.
/// @return pointer to Entity retrieved, or zero if it was not found.
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
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

void BaseWorld::setDefaultLocation(LocatedEntity* entity)
{
    m_defaultLocation = entity;
}

LocatedEntity* BaseWorld::getLimboLocation() const
{
    return m_limboLocation;
}

void BaseWorld::setLimboLocation(LocatedEntity* entity)
{
    m_limboLocation = entity;
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

double BaseWorld::getTime() const {
    SystemTime time{};
    time.update();
    return (double)(time.seconds() + timeoffset - m_initTime) + (double)time.microseconds()/1000000.;
}

