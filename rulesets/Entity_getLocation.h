// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Entity.h"

#include "common/debug.h"
#include "common/log.h"

#include <wfmath/atlasconv.h>

template <class EntityType>
bool Entity::getLocation(const Atlas::Message::MapType & entmap,
                         const std::map<std::string, EntityType *> & eobjects)
{
    debug( std::cout << "Entity::getLocation" << std::endl << std::flush;);
    Atlas::Message::MapType::const_iterator I = entmap.find("loc");
    if ((I == entmap.end()) || !I->second.isString()) {
        debug( std::cout << getId() << ".. has no loc" << std::endl << std::flush;);
        return true;
    }
    try {
        const std::string & ref_id = I->second.asString();
        typename std::map<std::string, EntityType *>::const_iterator J = eobjects.find(ref_id);
        if (J == eobjects.end()) {
            debug( std::cout << "ERROR: Can't get ref from objects dictionary" << std::endl << std::flush;);
            return true;
        }
            
        m_location.m_loc = J->second;
        I = entmap.find("pos");
        if (I != entmap.end()) {
            m_location.m_pos.fromAtlas(I->second.asList());
        }
        I = entmap.find("velocity");
        if (I != entmap.end()) {
            m_location.m_velocity.fromAtlas(I->second.asList());
        }
        I = entmap.find("orientation");
        if (I != entmap.end()) {
            m_location.m_orientation.fromAtlas(I->second.asList());
        }
        // bbox is no longer read here, because it needs to be handled
        // in set(), which means it gets read by merge
        // I = entmap.find("bbox");
        // if (I != entmap.end()) {
            // m_location.m_bBox.fromAtlas(I->second.asList());
        // }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "getLocation: Bad location data");
        return true;
    }
    return false;
}
