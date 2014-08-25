/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef LIMBOPROPERTY_H_
#define LIMBOPROPERTY_H_

#include "common/Property.h"

/**
 * When put on an entity, marks that entity as "limbo", i.e. place where non-controlled characters
 * are put when they die. When the characters then again becomes controlled they will be moved
 * from limbo to their respawn point.
 */
class LimboProperty : public Property<int>
{
    public:
        LimboProperty();

      virtual LimboProperty * copy() const;

      virtual void install(LocatedEntity *, const std::string &);
      virtual void remove(LocatedEntity *, const std::string &);
};

#endif /* LIMBOPROPERTY_H_ */
