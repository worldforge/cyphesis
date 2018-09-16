/*
 Copyright (C) 2015 Erik Ogenvik

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
#ifndef AICLIENT_MINDREGISTRY_H
#define AICLIENT_MINDREGISTRY_H

#include <modules/Ref.h>
#include "common/OperationRouter.h"

class BaseMind;

class MindRegistry {

    public:

        virtual ~MindRegistry() = default;

        virtual void addLocatedEntity(Ref<BaseMind>) = 0;
        virtual void removeLocatedEntity(Ref<BaseMind>) = 0;

        virtual void addPendingMind(std::string entityId, std::string mindId, OpVector& res) = 0;
        virtual void removePendingMind(std::string mindId) = 0;

};

#endif //AICLIENT_MINDREGISTRY_H
