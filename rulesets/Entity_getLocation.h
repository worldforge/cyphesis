// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

#error This file has been removed from the build

#include "Entity.h"

#include "common/debug.h"
#include "common/log.h"

#include <wfmath/atlasconv.h>

template <class EntityType>
bool Entity::getLocation(const Atlas::Objects::Entity::RootEntity & ent,
                         const std::map<std::string, EntityType *> & eobjects)
{
    debug( std::cout << "Entity::getLocation" << std::endl << std::flush;);
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        debug( std::cout << getId() << ".. has no loc" << std::endl << std::flush;);
        return true;
    }
    try {
        const std::string & ref_id = ent->getLoc();
        typename std::map<std::string, EntityType *>::const_iterator J = eobjects.find(ref_id);
        if (J == eobjects.end()) {
            debug( std::cout << "ERROR: Can't get ref from objects dictionary" << std::endl << std::flush;);
            return true;
        }
            
        m_location.m_loc = J->second;
        if (ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(m_location.m_pos, ent->getPos());
        }
        if (ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            fromStdVector(m_location.m_pos, ent->getVelocity());
        }
        Atlas::Message::Element orientation;
        if (ent->copyAttr("orientation", orientation) == 0) {
            if (orientation.isList()) {
                m_location.m_orientation.fromAtlas(orientation);
            } else {
                log(ERROR, "Malformed ORIENTATION data");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "getLocation: Bad location data");
        return true;
    }
    return false;
}
