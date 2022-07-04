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

#ifndef CYPHESIS_ADMINMIND_H
#define CYPHESIS_ADMINMIND_H


#include "ExternalMind.h"

/**
 * This mind subclass has additional capabilities and is allowed to directly alter the simulation.
 */
class AdminMind : public ExternalMind
{
    public:
        explicit AdminMind(RouterId id, Ref<LocatedEntity> entity);

        ~AdminMind() override = default;

        void externalOperation(const Operation& op, Link&) override;

        void addToEntity(const Atlas::Objects::Entity::RootEntity&) const override;

        void GetOperation(const Operation& smartPtr, OpVector& res) override;

};


#endif //CYPHESIS_ADMINMIND_H
