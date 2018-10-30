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

#ifndef CYPHESIS_HIT_H
#define CYPHESIS_HIT_H

#include <Atlas/Objects/Operation.h>

namespace Atlas {
    namespace Objects {
        namespace Operation {

            extern int HIT_NO;

            /// \brief A hit operation used to signal that an entity has been hit by something (a weapon for example).
            ///
            /// \ingroup CustomOperations
            class Hit : public Affect
            {
                public:
                    Hit()
                    {
                        (*this)->setType("hit", HIT_NO);
                    }
            };

        }
    }
}


#endif //CYPHESIS_HIT_H
