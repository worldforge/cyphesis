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

#ifndef CYPHESIS_VISIBILITY_H
#define CYPHESIS_VISIBILITY_H

/**
 * Visibility for entities, types and properties.
 *
 * When communicating information to clients we use these enums to protect some data.
 */
enum class Visibility
{
        /**
        * Sent to all.
        */
            PUBLIC,
        /**
        * Sent to the entity itself and admins.
        */
            PROTECTED,

        /**
        * Sent only to admins (not even the entity itself).
        */
            PRIVATE
};

#endif //CYPHESIS_VISIBILITY_H
