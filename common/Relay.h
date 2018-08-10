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

#ifndef CYPHESIS_RELAY_H
#define CYPHESIS_RELAY_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

    extern int RELAY_NO;

    /**
     * @brief A relayed operation.
     *
     * These operations are used for sending and responding between mainly Minds.
     * A Mind can send a Relay op to another Entity, directed to one of its Minds,
     * and get the Mind's response in return.
     *
     * @ingroup CustomOperations
     */
    class Relay : public Generic
    {
        public:
            Relay() {
                (*this)->setType("relay", RELAY_NO);
            }
    };

} } }
#endif //CYPHESIS_RELAY_H
