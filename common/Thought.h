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

#ifndef CYPHESIS_THOUGHT_H
#define CYPHESIS_THOUGHT_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

    extern int THOUGHT_NO;

    /// \brief A thought sent from a mind.
    ///
    /// Whenever a mind wants to control an entity this is done through Thoughts.

    /// \ingroup CustomOperations
    class Thought : public Generic
    {
        public:
            Thought()
            {
                (*this)->setType("thought", THOUGHT_NO);
            }
    };

} } }

#endif //CYPHESIS_THOUGHT_H
