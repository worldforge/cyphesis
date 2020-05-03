/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_ENTITYLOCATION_H
#define CYPHESIS_ENTITYLOCATION_H

#include "modules/Ref.h"

#include <wfmath/point.h>

class LocatedEntity;

/**
 * Represents a location in an entity. The position is optional; if it's invalid the location only
 * refers to the entity.
 */
struct EntityLocation
{
    /**
     * The parent entity.
     */
    Ref<LocatedEntity> m_parent;
    /**
     * Coords relative to the entity
     */
    WFMath::Point<3> m_pos;

    EntityLocation();

    explicit EntityLocation(Ref<LocatedEntity> loc);

    EntityLocation(Ref<LocatedEntity> loc, const WFMath::Point<3>& pos);

    const WFMath::Point<3>& pos() const;

    /**
     * True if there's both an entity as well as a valid position.
     * @return 
     */
    bool isValid() const;

    bool operator==(const EntityLocation& rhs) const
    {
        return m_parent == rhs.m_parent
               && m_pos == rhs.m_pos;
    }

    bool operator!=(const EntityLocation& rhs) const
    {
        return !(*this == rhs);
    }
};


#endif //CYPHESIS_ENTITYLOCATION_H
