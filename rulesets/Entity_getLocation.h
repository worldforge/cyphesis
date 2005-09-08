// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
