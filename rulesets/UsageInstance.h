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

#ifndef CYPHESIS_USAGEINSTANCE_H
#define CYPHESIS_USAGEINSTANCE_H

#include "common/Property.h"
#include "entityfilter/Filter.h"
#include "modules/Ref.h"
#include "EntityLocation.h"
#include <Atlas/Objects/RootOperation.h>
#include <memory>

struct Usage
{
    std::string description;

    std::vector<std::shared_ptr<EntityFilter::Filter>> targets;
    std::vector<std::shared_ptr<EntityFilter::Filter>> consumed;
    /**
     * The Python script which will handle this op.
     */
    std::string handler;
    std::shared_ptr<EntityFilter::Filter> constraint;

};

class UsageInstance
{
    public:
        Usage definition;

        Ref<LocatedEntity> actor;
        Ref<LocatedEntity> tool;

        std::vector<EntityLocation> targets;
        std::vector<EntityLocation> consumed;

        Atlas::Objects::Operation::RootOperation op;

        std::pair<bool, std::string> isValid() const;

};


#endif //CYPHESIS_USAGEINSTANCE_H
